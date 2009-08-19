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

#include "squirrel_thread.hpp"

#include "app/globals.hpp"
#include "scripting/util.hpp"
#include "scripting/squirrel_error.hpp"
#include "scripting/game_objects.hpp"

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

void
SquirrelThread::create_thread()
{
  assert(!thread);

  thread = sq_newthread(parent_vm, 1024);  // create thread and store it on the stack

  if (thread == 0)
    {
      throw SquirrelError(thread, filename, "Couldn't create new THREAD");
    }
  else
    { // Create a HSQOBJECT to hold a reference to the thread

      sq_resetobject(&thread_obj); // init the object

      // store thread created by sq_newthread into thread_obj
      if (sq_getstackobj(parent_vm, -1, &thread_obj) < 0)
        {
          throw SquirrelError(parent_vm, filename, "Couldn't get coroutine thread from stack");
        }
      else
        {
          sq_addref(parent_vm, &thread_obj); // add reference
          sq_pop(parent_vm, 1); // remove the thread from the stack
        }
    }
}

SquirrelThread::SquirrelThread(HSQUIRRELVM parent_vm, bool isolated_)
  : parent_vm(parent_vm),
    isolated(isolated_),
    thread(0),
    filename("<unset>"),
    oldtop(-1),
    waiting_for_events(ScriptManager::NO_EVENT),
    wakeup_time(0)
{
  create_thread();

  std::cout << "SquirrelThread: isolated = " << isolated << std::endl;
  if (isolated)
    { // create a local environment for the thread
      HSQOBJECT env;
      sq_resetobject(&env);

      sq_newtable(thread);

      // store the object in env
      if(sq_getstackobj(thread, -1, &env) < 0) 
        {
          throw SquirrelError(parent_vm, filename, "couldn't get table from stack");
        }
      else
        {
          sq_addref(thread, &env); 
          sq_pop(thread, 1); // remove env from stack
    
          // set old roottable as delegate on env
          sq_pushobject(thread, env); // push env
          sq_pushroottable(thread);   // [env, root]
          sq_setdelegate(thread, -2); // env.set_delegate(root)
          sq_pop(thread, 1);          // pop env

          // set env as new roottable
          sq_pushobject(thread, env);
          sq_setroottable(thread);

          sq_release(thread, &env);
        }
    }
}

void
SquirrelThread::load(std::istream& in, const std::string& filename_)
{
  filename = filename_;

  oldtop = sq_gettop(thread);

  // compile the script and push it on the stack
  if(SQ_FAILED(sq_compile(thread, squirrel_read_char, &in, filename.c_str(), SQTrue)))
    {
      throw SquirrelError(thread, filename, "Couldn't parse script");
    }
  else
    {
      // start the script that was previously compiled
      sq_pushroottable(thread);
      if (SQ_FAILED(sq_call(thread, 1, SQFalse, SQTrue)))
        {
          sq_pop(thread, 1); // pop the compiled closure
          throw SquirrelError(thread, filename, "SquirrelThread::load(): Couldn't start script");
        }
      else
        {
          if (sq_getvmstate(thread) != SQ_VMSTATE_IDLE)
            {
              throw SquirrelError(thread, filename, "SquirrelThread::load(): Script '" + filename +
                                  "' must not suspend outside of run call");
            }
          sq_pop(thread, 1); // pop the compiled closure
        }
    }
}

void
SquirrelThread::load(HSQUIRRELVM vm, SQInteger idx)
{
  oldtop = sq_gettop(thread);

  sq_move(thread, vm, idx);
  // FIXME: Could check object type here

  // Execute the function
  sq_pushroottable(thread);
  if (SQ_FAILED(sq_call(thread, 1, SQFalse, SQTrue)))
    {
      if(sq_getvmstate(thread) == SQ_VMSTATE_IDLE)
        { // Cleanup stack
          sq_settop(thread, oldtop);
        }

      throw SquirrelError(thread, filename, "SquirrelThread::run(): Couldn't start script");
    }
  else
    {
      if(sq_getvmstate(thread) == SQ_VMSTATE_IDLE)
        sq_settop(thread, oldtop);
    }
}

SquirrelThread::~SquirrelThread()
{
  sq_release(parent_vm, &thread_obj);
}

void
SquirrelThread::set_wakeup_event(const ScriptManager::WakeupEvent& event, float timeout)
{
  set_wakeup_event(ScriptManager::WakeupData(event), timeout);
}

void
SquirrelThread::set_wakeup_event(const ScriptManager::WakeupData& event, float timeout)
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
SquirrelThread::fire_wakeup_event(const ScriptManager::WakeupData& event)
{ 
  if (waiting_for_events.type == event.type && 
      waiting_for_events.type != ScriptManager::NO_EVENT)
    {
      switch (event.type)
        {
          case ScriptManager::SPEECH_DONE:
            if (waiting_for_events.id == event.id)
              {
                wakeup_time = game_time;
              }
            break;

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
SquirrelThread::update()
{
  int thread_state = sq_getvmstate(thread);
    
  switch(thread_state)
    {
      case SQ_VMSTATE_SUSPENDED:
        if (wakeup_time > 0 && game_time >= wakeup_time)
          {
            waiting_for_events = ScriptManager::WakeupData(ScriptManager::NO_EVENT);

            try 
              {
                // Try to return a value
                if (sq_wakeupvm(thread, SQFalse, SQFalse, SQTrue) < 0)
                  {
                    throw SquirrelError(thread, filename, "Couldn't resume script");
                  }
                else
                  {
                    if(sq_getvmstate(thread) == SQ_VMSTATE_IDLE) 
                      { // Cleanup stack
                        sq_settop(thread, oldtop);
                      }
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
SquirrelThread::is_suspended() const
{
  int thread_state = sq_getvmstate(thread);
  return thread_state == SQ_VMSTATE_SUSPENDED;
}

bool
SquirrelThread::is_idle() const
{
  int thread_state = sq_getvmstate(thread);
  return thread_state == SQ_VMSTATE_IDLE;
}

void
SquirrelThread::call(const std::string& function)
{
  if (0)
    { // Debug stuff that prints the current roottable and its delegate
      sq_pushroottable(thread);
      std::cout << ".------------------------------------------" << std::endl;
      std::cout << "################\nRootTable:\n{{{" << Scripting::squirrel2string(thread, -1) << "\n}}}" << std::endl;
      sq_getdelegate(thread, -1);
      std::cout << "################\nDelegate:\n{{{" << Scripting::squirrel2string(thread, -1) << "\n}}}" << std::endl;
      sq_pop(thread, 2);
      std::cout << "'------------------------------------------" << std::endl;
    }

  //std::cout << "SquirrelThread::call(\"" << function << "\")" << std::endl;

  oldtop = sq_gettop(thread);

  // Lookup the function in the roottable and put it on the stack
  sq_pushroottable(thread);
  sq_pushstring(thread, function.c_str(), -1);
  if (SQ_SUCCEEDED(sq_get(thread, -2)))
    {
      sq_pushroottable(thread);

      if (SQ_FAILED(sq_call(thread, 1, SQFalse, SQTrue)))
        {
          sq_settop(thread, oldtop);
          throw SquirrelError(thread, filename, "SquirrelThread: couldn't call '" + function + "'");
        }
      else
        {
          if(sq_getvmstate(thread) != SQ_VMSTATE_SUSPENDED) 
            {
              sq_settop(thread, oldtop);
            }
        }
    }
  else
    {
      sq_settop(thread, oldtop);

      if (0)
        std::cout << filename << ": Function '" << function << "' not found in roottable" << std::endl;
    }
}

/* EOF */
