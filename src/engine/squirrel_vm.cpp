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

#include "scripting/util.hpp"
#include "scripting/squirrel_error.hpp"
#include "squirrel_vm.hpp"

using Scripting::SquirrelError;

static SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);

  char c = in->get();

  if (in->eof())
    return 0;
  else
    return c;
}

SquirrelVM::SquirrelVM(std::istream& in, const std::string& arg_name, HSQUIRRELVM parent_vm)
  : name(arg_name),
    parent_vm(parent_vm),
    waiting_for_events(ScriptManager::NO_EVENT),
    wakeup_time(0)
{
  vm = sq_newthread(parent_vm, 1024);

  if (vm == 0)
    {
      throw SquirrelError(vm, name, "Couldn't create new VM");
    }
  else
    {
      // retrieve reference to thread from stack and increase refcounter

      // Init the object
      sq_resetobject(&vm_obj);

      // store thread created by sq_newthread into vm_obj
      if(sq_getstackobj(parent_vm, -1, &vm_obj) < 0)
        {
          throw SquirrelError(parent_vm, name, "Couldn't get coroutine vm from stack");
        }
      else
        {
          // Add reference and remove object from stack
          sq_addref(parent_vm, &vm_obj);
          sq_pop(parent_vm, 1);
        }
      
      HSQOBJECT env;
      sq_resetobject(&env);

      sq_newtable(vm);
      if(sq_getstackobj(vm, -1, &env) < 0) throw SquirrelError(parent_vm, name, "Couldn't get table from stack");
      sq_addref(vm, &env); // FIXME: needs to be freed

      // Create a completly empty environment and set delegate it to the roottable
      sq_pushobject(vm, env); // table
      sq_pushroottable(vm);   // table, root
      sq_setdelegate(vm, -2); // table.set_delegate(root)
      sq_pop(vm, 1);          //

      sq_pushobject(vm, env);
      sq_setroottable(vm);

      // FIXME: Belongs in run(), but there we don't have the 'in' anymore
      // Compile the script and push it on the stack
      if(sq_compile(vm, squirrel_read_char, &in, name.c_str(), true) < 0)
        throw SquirrelError(vm, name, "Couldn't parse script");

      sq_pushroottable(vm);

      // Start the script that was previously compiled
      if (SQ_FAILED(sq_call(vm, 1, false, true)))
        throw SquirrelError(vm, name, "SquirrelVM::run(): Couldn't start script");
    }
}

SquirrelVM::~SquirrelVM()
{
  sq_release(vm, &vm_obj);  
}

void
SquirrelVM::set_wakeup_event(const ScriptManager::WakeupEvent& event, float timeout)
{
  set_wakeup_event(ScriptManager::WakeupData(event), timeout);
}

void
SquirrelVM::set_wakeup_event(const ScriptManager::WakeupData& event, float timeout)
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
SquirrelVM::fire_wakeup_event(const ScriptManager::WakeupData& event)
{ 
  if (waiting_for_events.type == event.type && 
      waiting_for_events.type != ScriptManager::NO_EVENT)
    {
      switch (event.type)
        {
          case ScriptManager::GAMEOBJECT_DONE:
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

void
SquirrelVM::update()
{
  int vm_state = sq_getvmstate(vm);
    
  switch(vm_state)
    {
      case SQ_VMSTATE_SUSPENDED:
        if (wakeup_time > 0 && 
            game_time >= wakeup_time) 
          {
            waiting_for_events = ScriptManager::WakeupData(ScriptManager::NO_EVENT);

            try 
              {
                if (sq_wakeupvm(vm, false, false, true) < 0)
                  {
                    throw SquirrelError(vm, name, "Couldn't resume script");
                  }
              }
            catch(std::exception& e) 
              {
                std::cerr << "Problem executing script: " << e.what() << "\n";
              }
          }
        break;

      case SQ_VMSTATE_IDLE:
        break;

      case SQ_VMSTATE_RUNNING: // FIXME: Can this happen without multithreading?
        break;

      default: 
        assert(!"never reached");
    }
}

bool
SquirrelVM::is_suspended() const
{
  int vm_state = sq_getvmstate(vm);
  return vm_state == SQ_VMSTATE_SUSPENDED;
}

bool
SquirrelVM::is_idle() const
{
  int vm_state = sq_getvmstate(vm);
  return vm_state == SQ_VMSTATE_IDLE;
}

void
SquirrelVM::call(const std::string& function)
{
  sq_pushroottable(vm);

  //std::cout << "################\nRootTable:\n{{{" << Scripting::squirrel2string(vm, -1) << "\n}}}" << std::endl;
  sq_getdelegate(vm, -1);
  //std::cout << "################\nDelegate:\n{{{" << Scripting::squirrel2string(vm, -1) << "\n}}}" << std::endl;
  sq_pop(vm, 1);

  // Lookup the function in the roottable and put it on the stack
  sq_pushstring(vm, function.c_str(), -1);
  if (SQ_SUCCEEDED(sq_get(vm, -2)))
    {
      // Call the function
      sq_pushroottable(vm); // 'this' (function environment object)
      if (SQ_FAILED(sq_call(vm, 1, false, true)))
        {
          // FIXME: doesn't this mess up the stack?
          throw SquirrelError(vm, name, "SquirrelVM: couldn't call '" + function + "'");
        }

      // Cleanup
      sq_pop(vm, 2); //pops the roottable and the function
    }
  else
    {
      sq_pop(vm, 1); //pops the roottable
    }
}

/* EOF */
