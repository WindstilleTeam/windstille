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

#include "squirrel_vm.hpp"
#include "app/console.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/util.hpp"
#include "scripting/squirrel_error.hpp"
#include "physfs/physfs_stream.hpp"

using Scripting::SquirrelError;

// The table (works like a namespace here) where the game objects will appear
static const char* OBJECTS_TABLE = "objects";

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

      // Create the "objects" table
      sq_pushroottable(vm);
      sq_pushstring(vm, OBJECTS_TABLE, -1);
      sq_newtable(vm);
      sq_newslot(vm, -3, SQFalse);
      sq_pop(vm, 1);
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

void
ScriptManager::remove_object_from_squirrel(boost::shared_ptr<GameObject> object)
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
static inline void create_squirrel_instance(HSQUIRRELVM v, boost::shared_ptr<GameObject> object)
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
ScriptManager::expose_object_to_squirrel(boost::shared_ptr<GameObject> object)
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
