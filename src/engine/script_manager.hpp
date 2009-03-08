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

#ifndef HEADER_WINDSTILLE_ENGINE_SCRIPT_MANAGER_HPP
#define HEADER_WINDSTILLE_ENGINE_SCRIPT_MANAGER_HPP

#include <vector>
#include <list>
#include <map>
#include <squirrel.h>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "scripting/wrapper.hpp"
#include "scripting/util.hpp"

class SquirrelVM;

/**
 * This class is responsible for managing all running squirrel threads
 * (called coroutines by others)
 * It contains functions for loading and starting script, keeps a list of
 * suspended scripts and receives wakeup events for them
 */
class ScriptManager
{
private:
  static ScriptManager* current_;
public:
  static ScriptManager* current() { return current_; }

public:
  enum WakeupEvent 
    {
      NO_EVENT,
      TIME,
      DIALOG_CLOSED,
      CONVERSATION_CLOSED,
      FADE_DONE,
      CAMERA_DONE,
      GAMEOBJECT_DONE,
      MAX_WAKEUP_EVENT_COUNT
    };

  struct WakeupData 
  {
    explicit WakeupData() : type(NO_EVENT) {}
    explicit WakeupData(WakeupEvent type_) : type(type_) {}

    WakeupEvent type;
    
    union {
      // GAMEOBJECT_DONE
      GameObject* game_object;
    };
  };

private:  
  typedef std::list<boost::shared_ptr<SquirrelVM> > SquirrelVMs;
  SquirrelVMs squirrel_vms;
  std::map<std::string, bool> already_run_scripts;

  HSQUIRRELVM vm;

public:
  ScriptManager();
  ~ScriptManager();

  void update();

  /** Load script from \a filename and runs it */
  void run_script_file(const std::string& filename);

  /** Takes the Squirrel expression in \a script and evaluates it, \a
      sourcename is the filename and used in error messages */
  void run_script(const std::string& script, const std::string& sourcename);

  /** Load script from \a in and runs it, \a sourcename is the
      filename and used in error messages */
  void run_script(std::istream& in, const std::string& sourcename);

  HSQUIRRELVM get_vm() const { return vm; }

  void set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float timeout = -1);
  void set_wakeup_event(HSQUIRRELVM vm, WakeupData  event, float timeout = -1);

  void fire_wakeup_event(WakeupEvent event);
  void fire_wakeup_event(WakeupData  event);
  
  /** Returns true if the given vm as been used in a run_script() call
      before, if the vm is run for the first time return true */
  bool run_before(HSQUIRRELVM vm);
};

#endif

/* EOF */
