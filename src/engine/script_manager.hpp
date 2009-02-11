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

#ifndef HEADER_WINDSTILLE_SCRIPT_MANAGER_HPP
#define HEADER_WINDSTILLE_SCRIPT_MANAGER_HPP

#include <vector>
#include <list>
#include <map>
#include <squirrel.h>
#include <iostream>
#include "timer.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/util.hpp"

/**
 * This class is responsible for managing all running squirrel threads
 * (called coroutines by others)
 * It contains functions for loading and starting script, keeps a list of
 * suspended scripts and receives wakeup events for them
 */
class ScriptManager
{
public:
  ScriptManager();
  ~ScriptManager();

  void update();

  void run_script_file(const std::string& filename);
  void run_script(const std::string& string, const std::string& sourcename);
  void run_script(std::istream& in, const std::string& sourcename);

  HSQUIRRELVM get_vm() const
  {
    return v;
  }

  enum WakeupEvent {
    NO_EVENT,
    TIME,
    DIALOG_CLOSED,
    CONVERSATION_CLOSED,
    FADE_DONE,
    CAMERA_DONE,
    GAMEOBJECT_DONE,
    MAX_WAKEUP_EVENT_COUNT
  };

  struct WakeupData {
    explicit WakeupData() : type(NO_EVENT) {}
    explicit WakeupData(WakeupEvent type_) : type(type_) {}

    WakeupEvent type;
    
    union {
      // GAMEOBJECT_DONE
      GameObject* game_object;
    };
  };

  void set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float timeout = -1);
  void set_wakeup_event(HSQUIRRELVM vm, WakeupData  event, float timeout = -1);
  void fire_wakeup_event(WakeupEvent event);
  void fire_wakeup_event(WakeupData  event);
  
  bool run_before(HSQUIRRELVM vm);

private:
  class SquirrelVM
  {
  public:
    SquirrelVM(const std::string& arg_name, HSQUIRRELVM arg_vm, HSQOBJECT arg_obj);
    std::string name;
    HSQUIRRELVM vm;
    HSQOBJECT   vm_obj;
    float       wakeup_time;
    WakeupData  waiting_for_events;
  };
  
  typedef std::list<SquirrelVM> SquirrelVMs;
  SquirrelVMs squirrel_vms;
  std::map<std::string, bool> already_run_scripts;

  HSQUIRRELVM v;
};

extern ScriptManager* script_manager;

#endif

/* EOF */
