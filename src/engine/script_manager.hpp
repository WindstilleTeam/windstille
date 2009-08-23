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

#include "util/currenton.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/util.hpp"

class SquirrelThread;
class GameObject;

/**
 * This class is responsible for managing all running squirrel threads
 * (called coroutines by others)
 * It contains functions for loading and starting script, keeps a list of
 * suspended scripts and receives wakeup events for them
 */
class ScriptManager : public Currenton<ScriptManager>
{
public:
  enum WakeupEvent 
    {
      NO_EVENT,
      TIME,
      DIALOG_CLOSED,
      CONVERSATION_CLOSED,
      SPEECH_DONE,
      FADE_DONE,
      CAMERA_DONE,
      GAMEOBJECT_DONE,
      MAX_WAKEUP_EVENT_COUNT
    };

#pragma GCC diagnostic warning "-Weffc++"
  struct WakeupData 
  {
    explicit WakeupData() : type(NO_EVENT) {}
    explicit WakeupData(WakeupEvent type_) : type(type_) {}

    WakeupEvent type;
    
    union {
      // GAMEOBJECT_DONE
      GameObject* game_object;
      int id;
    };
  };

private:  
  typedef std::list<boost::shared_ptr<SquirrelThread> > SquirrelThreads;
  SquirrelThreads squirrel_vms;

  HSQUIRRELVM vm;

public:
  ScriptManager();
  ~ScriptManager();

  void update();

  /** Load script from \a filename and runs it */
  boost::shared_ptr<SquirrelThread> run_script_file(const std::string& filename, bool global = false);
  boost::shared_ptr<SquirrelThread> create_script(HSQUIRRELVM parent_vm, bool isolated);

  HSQUIRRELVM get_vm() const { return vm; }

  boost::shared_ptr<SquirrelThread> get_thread(HSQUIRRELVM v) const;

  void fire_wakeup_event(WakeupEvent event);
  void fire_wakeup_event(WakeupData  event);
  
  void expose_object_to_squirrel(boost::shared_ptr<GameObject> object);
  void remove_object_from_squirrel(boost::shared_ptr<GameObject> object);

private:
  ScriptManager(const ScriptManager&);
  ScriptManager& operator=(const ScriptManager&);
};

#endif

/* EOF */
