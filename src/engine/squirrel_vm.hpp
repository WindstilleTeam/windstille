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

#ifndef HEADER_WINDSTILLE_ENGINE_SQUIRREL_VM_HPP
#define HEADER_WINDSTILLE_ENGINE_SQUIRREL_VM_HPP

#include <string>
#include <squirrel.h>

#include "script_manager.hpp"

class SquirrelVM
{
public:
  std::string name;
private:
  HSQUIRRELVM parent_vm;
public:
  HSQUIRRELVM vm;
private:
  HSQOBJECT   vm_obj;
    
  ScriptManager::WakeupData  waiting_for_events;
  float       wakeup_time;

public:
  SquirrelVM(std::istream& in, const std::string& arg_name, HSQUIRRELVM parent_vm);
  ~SquirrelVM();

  void run();

  void set_wakeup_event(const ScriptManager::WakeupData& event, float timeout);
  void fire_wakeup_event(const ScriptManager::WakeupData& event);

  /** Returns false when the VM is done and can be removed */
  bool update();
};

#endif

/* EOF */
