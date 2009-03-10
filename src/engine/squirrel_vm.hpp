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
private:
  std::string name;
  HSQUIRRELVM parent_vm;
  HSQUIRRELVM vm;
  HSQOBJECT   vm_obj;
    
  ScriptManager::WakeupData  waiting_for_events;
  float       wakeup_time;

public:
  SquirrelVM(std::istream& in, const std::string& arg_name, HSQUIRRELVM parent_vm);
  ~SquirrelVM();

  void set_wakeup_event(const ScriptManager::WakeupData&  event, float timeout = -1);
  void set_wakeup_event(const ScriptManager::WakeupEvent& event, float timeout = -1);

  void fire_wakeup_event(const ScriptManager::WakeupData& event);

  /** Resumes the evaluation of the VM if a wakeup has happened.
      \return false when the VM is done and can be removed */
  void update();

  bool is_suspended() const;
  bool is_idle() const;

  std::string get_name() const { return name; }
  HSQUIRRELVM get_vm() const { return vm; }
  void call(const std::string& function);
};

#endif

/* EOF */
