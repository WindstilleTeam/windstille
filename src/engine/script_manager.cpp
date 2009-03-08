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

ScriptManager* ScriptManager::current_ = 0;

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
  assert(current_ == 0);
  current_ = this;

  vm = sq_open(1024);
  if(vm == 0)
    {
      throw std::runtime_error("Couldn't initialize squirrel vm");
    }
  else
    {
      // register default error handlers
      sqstd_seterrorhandlers(vm);
      // register squirrel libs
      sq_pushroottable(vm);

      /* FIXME: None of these should be needed for scripts

         if(sqstd_register_bloblib(v) < 0)
         throw SquirrelError(v, "Couldn't register blob lib");

         if(sqstd_register_iolib(v) < 0)
         throw SquirrelError(v, "Couldn't register io lib");

         if(sqstd_register_systemlib(v) < 0)
         throw SquirrelError(v, "Couldn't register system lib");
      */

      if(sqstd_register_mathlib(vm) < 0)
        throw SquirrelError(vm, "Couldn't register math lib");

      if(sqstd_register_stringlib(vm) < 0)
        throw SquirrelError(vm, "Couldn't register string lib");

      // register print function
      sq_setprintfunc(vm, printfunc);
  
      // register windstille API
      Scripting::register_windstille_wrapper(vm);
    }
}

ScriptManager::~ScriptManager()
{
  // Manually clear it so that the boost::shared_ptr get deleted
  // before we close the vm
  squirrel_vms.clear();

  sq_close(vm);
  current_ = 0;
}

static SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);

  char c = in->get();

  if (in->eof())
    return 0;
  else
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
  // Add VM to the list of VMs
  squirrel_vms.push_back(boost::shared_ptr<SquirrelVM>(new SquirrelVM(in, sourcename, vm)));
  squirrel_vms.back()->run();
  already_run_scripts[sourcename] = true;   
}

void
ScriptManager::update()
{
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ) 
    {
      if ((*i)->update())
        {
          ++i;
        }
      else
        {
          i = squirrel_vms.erase(i);
        }
    }
}

void
ScriptManager::set_wakeup_event(HSQUIRRELVM vm, WakeupData event, float timeout)
{
  assert(event.type >= 0 && event.type < MAX_WAKEUP_EVENT_COUNT);

  // find the VM in the list and update it
  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) 
    {
      if((*i)->vm == vm) 
        {
          (*i)->set_wakeup_event(event, timeout);
          return;
        }
    }
}

void
ScriptManager::set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float timeout)
{
  set_wakeup_event(vm, WakeupData(event), timeout);
}

void
ScriptManager::fire_wakeup_event(WakeupData event)
{
  assert(event.type >= 0 && event.type < MAX_WAKEUP_EVENT_COUNT);

  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) 
    {
      (*i)->fire_wakeup_event(event);
    }
}

void
ScriptManager::fire_wakeup_event(WakeupEvent event)
{
  fire_wakeup_event(WakeupData(event));
}

bool
ScriptManager::run_before(HSQUIRRELVM vm)
{
  std::string name;

  for(SquirrelVMs::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i) 
    {
      if ((*i)->vm == vm)
        name = (*i)->name;
    }
  
  if (already_run_scripts.find(name) == already_run_scripts.end())
    return false;
  else
    return true;
}

ScriptManager::SquirrelVM::SquirrelVM(std::istream& in, const std::string& arg_name, HSQUIRRELVM parent_vm)
  : name(arg_name),
    parent_vm(parent_vm),
    waiting_for_events(NO_EVENT),
    wakeup_time(0)
{
  vm = sq_newthread(parent_vm, 1024);

  if (vm == 0)
    {
      throw SquirrelError(vm, "Couldn't create new VM");
    }
  else
    {
      // retrieve reference to thread from stack and increase refcounter

      // Init the object
      sq_resetobject(&vm_obj);

      // store thread created by sq_newthread into vm_obj
      if(sq_getstackobj(parent_vm, -1, &vm_obj) < 0)
        {
          throw SquirrelError(parent_vm, "Couldn't get coroutine vm from stack");
        }
      else
        {
          // Add reference and remove object from stack
          sq_addref(parent_vm, &vm_obj);
          sq_pop(parent_vm, 1);
  
          // Compile the script and push it on the stack
          if(sq_compile(vm, squirrel_read_char, &in, name.c_str(), true) < 0)
            throw SquirrelError(vm, "Couldn't parse script");
        }

      // FIXME: a script that gets run shouldn't have direct access to the root table
      // http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/MultiVMs.html
      sq_pushroottable(vm);
      //sq_clone(vm, -1); //FIXME
    }
}

ScriptManager::SquirrelVM::~SquirrelVM()
{
  sq_release(vm, &vm_obj);  
}

void 
ScriptManager::SquirrelVM::run()
{
  // Start the script that was previously compiled
  if (SQ_FAILED(sq_call(vm, 1, false, true)))
    throw SquirrelError(vm, "Couldn't start script");
}

void
ScriptManager::SquirrelVM::set_wakeup_event(const WakeupData& event, float timeout)
{
  waiting_for_events = event;

  if (timeout < 0) 
    {
      wakeup_time = -1;
    } 
  else 
    {
      wakeup_time = game_time + timeout;
    }
}

void
ScriptManager::SquirrelVM::fire_wakeup_event(const WakeupData& event)
{ 
  if (waiting_for_events.type == event.type && 
      waiting_for_events.type != NO_EVENT)
    {
      switch (event.type)
        {
          case GAMEOBJECT_DONE:
            if (waiting_for_events.game_object == event.game_object)
              {
                wakeup_time = game_time;
              }
            break;

          default:
            wakeup_time = game_time;
            break;
        }
    }
}

bool
ScriptManager::SquirrelVM::update()
{
  int vm_state = sq_getvmstate(vm);
    
  switch(vm_state)
    {
      case SQ_VMSTATE_SUSPENDED:
        if (wakeup_time > 0 && 
            game_time >= wakeup_time) 
          {
            waiting_for_events = WakeupData(NO_EVENT);

            try 
              {
                if(sq_wakeupvm(vm, false, false, true) < 0) 
                  {
                    throw SquirrelError(vm, "Couldn't resume script");
                  }
                return true;
              }
            catch(std::exception& e) 
              {
                std::cerr << "Problem executing script: " << e.what() << "\n";
                return false;
              }
          }
        else
          {
            return true;
          }
        break;

      case SQ_VMSTATE_IDLE:
      case SQ_VMSTATE_RUNNING: // FIXME: How can this happen?
        return false;

      default: 
        assert(!"never reached");
    }
}

/* EOF */
