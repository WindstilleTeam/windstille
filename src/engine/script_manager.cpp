#include "script_manager.hpp"

#include <stdarg.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <sqstdio.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdmath.h>
#include <sqstdstring.h>

#include "app/console.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/util.hpp"
#include "scripting/squirrel_error.hpp"
#include "physfs/physfs_stream.hpp"

using namespace Scripting;

ScriptManager* script_manager = 0;

static void printfunc(HSQUIRRELVM, const char* str, ...)
{
  char buf[4096];
  va_list arglist; 
  va_start(arglist, str); 
  vsprintf(buf, str, arglist);
  console << (char*)buf;
  puts(buf);
  va_end(arglist); 
}

ScriptManager::ScriptManager()
{
  v = sq_open(1024);
  if(v == 0)
    throw std::runtime_error("Couldn't initialize squirrel vm");

  // register default error handlers
  sqstd_seterrorhandlers(v);
  // register squirrel libs
  sq_pushroottable(v);
  if(sqstd_register_bloblib(v) < 0)
    throw SquirrelError(v, "Couldn't register blob lib");
  if(sqstd_register_iolib(v) < 0)
    throw SquirrelError(v, "Couldn't register io lib");
  if(sqstd_register_systemlib(v) < 0)
    throw SquirrelError(v, "Couldn't register system lib");
  if(sqstd_register_mathlib(v) < 0)
    throw SquirrelError(v, "Couldn't register math lib");
  if(sqstd_register_stringlib(v) < 0)
    throw SquirrelError(v, "Couldn't register string lib");

  // register print function
  sq_setprintfunc(v, printfunc);
  
  // register windstille API
  Scripting::register_windstille_wrapper(v);
}

ScriptManager::~ScriptManager()
{
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i)
    sq_release(v, &(i->vm_obj));

  sq_close(v);
}

static SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);
  char c = in->get();
  if(in->eof())
    return 0;    
  return c;
}

void
ScriptManager::run_script_file(const std::string& filename)
{
  IFileStream in(filename);
  run_script(in, filename);
}

void
ScriptManager::run_script(const std::string& the_string,
    const std::string& sourcename)
{
  std::istringstream stream(the_string);
  run_script(stream, sourcename);
}

void
ScriptManager::run_script(std::istream& in, const std::string& sourcename)
{
  HSQUIRRELVM vm = sq_newthread(v, 1024);
  if(vm == 0)
    throw SquirrelError(v, "Couldn't create new VM");

  // retrieve reference to thread from stack and increase refcounter
  HSQOBJECT vm_obj;
  sq_resetobject(&vm_obj);
  if(sq_getstackobj(v, -1, &vm_obj) < 0)
    throw SquirrelError(v, "Couldn't get coroutine vm from stack");
  sq_addref(v, &vm_obj);
  sq_pop(v, 1);
  
  if(sq_compile(vm, squirrel_read_char, &in, sourcename.c_str(), true) < 0)
    throw SquirrelError(vm, "Couldn't parse script");
	
  squirrel_vms.push_back(SquirrelVM(sourcename, vm, vm_obj));
  already_run_scripts[sourcename] = true;

  // FIXME: a script that gets run shouldn't have direct access to the root table
  // http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/MultiVMs.html
  sq_pushroottable(vm);
  //sq_clone(vm, -1); //FIXME
  if(sq_call(vm, 1, false, true) < 0)
    throw SquirrelError(vm, "Couldn't start script");
}

void
ScriptManager::update()
{
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ) {
    SquirrelVM& squirrel_vm = *i;
    int vm_state = sq_getvmstate(squirrel_vm.vm);
    
    if(vm_state == SQ_VMSTATE_SUSPENDED && squirrel_vm.wakeup_time > 0 && game_time >= squirrel_vm.wakeup_time) 
      {
        squirrel_vm.waiting_for_events = WakeupData(NO_EVENT);
        try {
          if(sq_wakeupvm(squirrel_vm.vm, false, false, true) < 0) {
            throw SquirrelError(squirrel_vm.vm, "Couldn't resume script");
          }
        } catch(std::exception& e) {
          std::cerr << "Problem executing script: " << e.what() << "\n";
          sq_release(v, &squirrel_vm.vm_obj);
          i = squirrel_vms.erase(i);
          continue;
        }
      }
	
    if (vm_state != SQ_VMSTATE_SUSPENDED)
      {
        sq_release(v, &(squirrel_vm.vm_obj));
        i = squirrel_vms.erase(i);
      }
    else
      {
        ++i;
      }
  }
}

void
ScriptManager::set_wakeup_event(HSQUIRRELVM vm, WakeupData event, float timeout)
{
  assert(event.type >= 0 && event.type < MAX_WAKEUP_EVENT_COUNT);
  // find the VM in the list and update it
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) {
    SquirrelVM& squirrel_vm = *i;
    if(squirrel_vm.vm == vm) 
      {
        squirrel_vm.waiting_for_events = event;

        if(timeout < 0) {
          squirrel_vm.wakeup_time = -1;
        } else {
          squirrel_vm.wakeup_time = game_time + timeout;
        }
        return;
      }
  }
}

void
ScriptManager::fire_wakeup_event(WakeupData  event)
{
  assert(event.type >= 0 && event.type < MAX_WAKEUP_EVENT_COUNT);
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) 
    {
      SquirrelVM& vm = *i;
      if(vm.waiting_for_events.type == event.type && vm.waiting_for_events.type != NO_EVENT)
        {
          switch (event.type)
            {
            case GAMEOBJECT_DONE:
              if (vm.waiting_for_events.game_object == event.game_object)
                {
                  vm.wakeup_time = game_time;
                }
              break;

            default:
              vm.wakeup_time = game_time;
              break;
            }
        }
    }
}

void
ScriptManager::set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float timeout)
{
  set_wakeup_event(vm, WakeupData(event), timeout);
}

void
ScriptManager::fire_wakeup_event(WakeupEvent event)
{
  fire_wakeup_event(WakeupData(event));
}

bool ScriptManager::run_before(HSQUIRRELVM vm)
{
  std::string name;
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) {
    if (i->vm == vm)
      name = i->name;
  }
  
  if (already_run_scripts.find(name) == already_run_scripts.end())
    return false;
  return true;
}

ScriptManager::SquirrelVM::SquirrelVM(const std::string& arg_name, HSQUIRRELVM arg_vm, HSQOBJECT arg_obj)
  : name(arg_name),
    vm(arg_vm), 
    vm_obj(arg_obj)
{
  waiting_for_events = WakeupData(NO_EVENT);
  wakeup_time        = 0;
}

/* EOF */
