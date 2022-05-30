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

#ifndef HEADER_WINDSTILLE_ENGINE_SQUIRREL_THREAD_HPP
#define HEADER_WINDSTILLE_ENGINE_SQUIRREL_THREAD_HPP

#include "engine/script_manager.hpp"
#include "util/pathname.hpp"

namespace windstille {


class SquirrelThread
{
private:
  HSQUIRRELVM parent_vm;
  bool isolated;

  HSQUIRRELVM thread;
  HSQOBJECT   thread_obj;

  Pathname filename;
  SQInteger oldtop;

  ScriptManager::WakeupData  waiting_for_events;
  float       wakeup_time;

  void create_thread();

public:
  SquirrelThread(HSQUIRRELVM parent_vm, bool isolated);
  ~SquirrelThread();

  /** Load a script into the new thread */
  void load(std::istream& in, Pathname const& filename);

  /** Load function at position \a idx into this SquirrelThread and
      execute it */
  void load(HSQUIRRELVM vm, SQInteger idx);

  void set_wakeup_event(ScriptManager::WakeupData const&  event, float timeout = -1);
  void set_wakeup_event(ScriptManager::WakeupEvent const& event, float timeout = -1);

  void fire_wakeup_event(ScriptManager::WakeupData const& event);

  /** Resumes the evaluation of the VM if a wakeup has happened.
      \return false when the VM is done and can be removed */
  void update();

  bool is_suspended() const;
  bool is_idle() const;

  Pathname get_filename() const { return filename; }
  HSQUIRRELVM get_thread() const { return thread; }
  void call(std::string const& function);

private:
  SquirrelThread(SquirrelThread const&);
  SquirrelThread& operator=(SquirrelThread const&);
};


} // namespace windstille

#endif

/* EOF */
