/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "engine/script_manager.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <sqstdaux.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <fstream>
#include <sstream>

#include "engine/squirrel_thread.hpp"
#include "app/console.hpp"
#include "scripting/game_objects.hpp"
#include "scripting/squirrel_error.hpp"

#ifndef __clang__
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
#else
#  pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

using Scripting::SquirrelError;

// The table (works like a namespace here) where the game objects will appear
static char const* OBJECTS_TABLE = "objects";

// FIXME: Replace this with sq_compilebufer()
static SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);

  char c = static_cast<char>(in->get());

  if (in->eof())
    return 0;
  else
    return c;
}

namespace {

void printfunc(HSQUIRRELVM, char const* str, ...)
{
  char buf[4096];
  va_list arglist;
  va_start(arglist, str);
  vsprintf(buf, str, arglist);
  ConsoleLog << static_cast<char*>(buf);
  puts(buf);
  va_end(arglist);
}

void errorfunc(HSQUIRRELVM, char const* str, ...)
{
  char buf[4096];
  va_list arglist;
  va_start(arglist, str);
  vsprintf(buf, str, arglist);
  ConsoleLog << static_cast<char*>(buf);
  puts(buf);
  va_end(arglist);
}

} // namespace

ScriptManager::ScriptManager() :
  squirrel_vms(),
  vm()
{
  vm = sq_open(1024);
  if(vm == nullptr)
  {
    throw std::runtime_error("Couldn't initialize squirrel vm");
  }
  else
  {
    // register default error handlers
    sqstd_seterrorhandlers(vm);

    { // register squirrel libs in the root table

      sq_pushroottable(vm);

      /* FIXME: None of these should be needed for scripts

         if(SQ_FAILED(sqstd_register_bloblib(v)))
         throw SquirrelError(v, "Couldn't register blob lib");

         if(SQ_FAILED(sqstd_register_iolib(v)))
         throw SquirrelError(v, "Couldn't register io lib");

         if(SQ_FAILED(sqstd_register_systemlib(v)))
         throw SquirrelError(v, "Couldn't register system lib");
      */

      if(SQ_FAILED(sqstd_register_mathlib(vm)))
        throw SquirrelError(vm, "Couldn't register math lib");

      if(SQ_FAILED(sqstd_register_stringlib(vm)))
        throw SquirrelError(vm, "Couldn't register string lib");

      // register print function
      sq_setprintfunc(vm, &printfunc, &errorfunc);

      // register windstille API
      Scripting::register_windstille_wrapper(vm);
      sq_pop(vm, 1);
    }

    // Create the empty "objects" table
    sq_pushroottable(vm);
    sq_pushstring(vm, OBJECTS_TABLE, -1);
    sq_newtable(vm);
    sq_newslot(vm, -3, SQFalse);
    sq_pop(vm, 1);
  }
}

ScriptManager::~ScriptManager()
{
  // Manually clear it so that the std::shared_ptr get deleted
  // before we close the vm
  squirrel_vms.clear();

  sq_close(vm);
}

std::shared_ptr<SquirrelThread>
ScriptManager::create_script(HSQUIRRELVM parent_vm, bool isolated)
{
  // Add VM to the list of VMs
  squirrel_vms.push_back(std::shared_ptr<SquirrelThread>(new SquirrelThread(parent_vm, isolated)));
  return squirrel_vms.back();
}

std::shared_ptr<SquirrelThread>
ScriptManager::run_script_file(Pathname const& filename, bool global)
{
  std::ifstream in(filename.get_sys_path().c_str());

  if (global)
  {
    // Scripts run in the global namespace must not suspend

    // Compile the script and push it on the stack
    if (sq_compile(vm, squirrel_read_char, &in, filename.get_sys_path().c_str(), true) < 0)
      throw SquirrelError(vm, "Couldn't parse script");

    // Set 'this' environment
    sq_pushroottable(vm);

    // Execute the script
    if (SQ_FAILED(sq_call(vm, 1, false, true)))
    {
      std::ostringstream str;
      str << "SquirrelThread::run(): " << filename << ": Couldn't start script";
      throw SquirrelError(vm, str.str());
    }
    else
    {
      sq_pop(vm, 1);

      if (sq_getvmstate(vm) != SQ_VMSTATE_IDLE)
      {
        std::ostringstream str;
        str << "ScriptManager::run_script(): '" << filename << "': global scripts must not suspend";
        throw std::runtime_error(str.str());
      }

      return std::shared_ptr<SquirrelThread>();
    }
  }
  else
  {
    SquirrelThreads::iterator it = squirrel_vms.end();

    // Look if the VM is associated with the source file
    for(SquirrelThreads::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i)
    {
      if ((*i)->get_filename() == filename)
      {
        it = i;
        break;
      }
    }

    if (it != squirrel_vms.end())
    {
      // Call the run method
      if ((*it)->is_idle())
      {
        (*it)->call("run");
        return *it;
      }
      else
      {
        std::ostringstream str;
        str << filename << ": ScriptManager::run_script(): Script must be idle to be 'run()'";
        throw std::runtime_error(str.str());
      }
    }
    else
    { // Add VM to the list of VMs
      squirrel_vms.push_back(std::shared_ptr<SquirrelThread>(new SquirrelThread(vm, true)));
      squirrel_vms.back()->load(in, filename);
      squirrel_vms.back()->call("init");
      squirrel_vms.back()->call("run");
      return squirrel_vms.back();
    }
  }
}

void
ScriptManager::update()
{
  for(SquirrelThreads::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i)
  {
    (*i)->update();
  }
}

std::shared_ptr<SquirrelThread>
ScriptManager::get_thread(HSQUIRRELVM v) const
{
  for(SquirrelThreads::const_iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i)
  {
    if ((*i)->get_thread() == v)
      return *i;
  }

  return std::shared_ptr<SquirrelThread>();
}

void
ScriptManager::fire_wakeup_event(WakeupData event)
{
  assert(event.type >= 0 && event.type < MAX_WAKEUP_EVENT_COUNT);

  for(SquirrelThreads::iterator i = squirrel_vms.begin(); i != squirrel_vms.end(); ++i)
  {
    (*i)->fire_wakeup_event(event);
  }
}

void
ScriptManager::fire_wakeup_event(WakeupEvent event)
{
  fire_wakeup_event(WakeupData(event));
}

void
ScriptManager::remove_object_from_squirrel(std::shared_ptr<GameObject> object)
{
  // get objects table
  HSQUIRRELVM v = ScriptManager::current()->get_vm();
  sq_pushroottable(v);
  sq_pushstring(v, OBJECTS_TABLE, -1);
  if(SQ_FAILED(sq_get(v, -2)))
  {
    std::ostringstream msg;
    msg << "Couldn't get objects table '" << OBJECTS_TABLE << "'";
    throw SquirrelError(v, msg.str());
  }

  // remove object from table
  sq_pushstring(v, object->get_name().c_str(), object->get_name().size());
  if(SQ_FAILED(sq_deleteslot(v, -2, SQFalse))) {
    std::ostringstream msg;
    msg << "Couldn't remove squirrel object for '" << object->get_name()
        << "'";
    throw SquirrelError(v, msg.str());
  }

  // pop objects and root table
  sq_pop(v, 2);
}

// tries to find out the "real" class of an gameobject by some dynamic casting
// and creates a matching squirrel instance for that object
static inline void create_squirrel_instance(HSQUIRRELVM v, std::shared_ptr<GameObject> object)
{
  if (dynamic_cast<ScriptableObject*>(object.get()))
  {
    create_squirrel_instance(v, new Scripting::ScriptableObject(object),
                             true);
    return;
  }

  if (dynamic_cast<TestObject*>(object.get()))
  {
    create_squirrel_instance(v, new Scripting::TestObject(object), true);
    return;
  }

  if (dynamic_cast<Player*>(object.get()))
  {
    create_squirrel_instance(v, new Scripting::Player(object), true);
    return;
  }

  create_squirrel_instance(v, new Scripting::GameObject(object), true);
}

void
ScriptManager::expose_object_to_squirrel(std::shared_ptr<GameObject> object)
{
  // get objects table
  HSQUIRRELVM v = ScriptManager::current()->get_vm();
  sq_pushroottable(v);
  sq_pushstring(v, OBJECTS_TABLE, -1);
  if(SQ_FAILED(sq_get(v, -2)))
  {
    std::ostringstream msg;
    msg << "Couldn't get objects table '" << OBJECTS_TABLE << "'";
    throw SquirrelError(v, msg.str());
  }

  // create squirrel instance and register in table
  sq_pushstring(v, object->get_name().c_str(), object->get_name().size());
  create_squirrel_instance(v, object);
  if(SQ_FAILED(sq_createslot(v, -3)))
  {
    std::ostringstream msg;
    msg << "Couldn't register object in objects taböe";
    throw SquirrelError(v, msg.str());
  }

  // pop roottable and objects table
  sq_pop(v, 2);
}

/* EOF */
