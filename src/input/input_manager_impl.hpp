/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_IMPL_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_IMPL_HPP

#include "controller.hpp"
#include "input_event.hpp"

/** */
class InputManagerImpl
{
protected:
  Controller controller;

public:
  InputManagerImpl() : controller() {}
  virtual ~InputManagerImpl() {}

  virtual void load(const std::string& filename) =0;
  virtual void update(float delta) =0;
  
  const Controller& get_controller() const;
  void clear();

  virtual void add_axis_event  (int name, float pos);
  virtual void add_ball_event  (int name, float pos);
  virtual void add_button_event(int name, bool down);
  virtual void add_keyboard_event(int name, KeyboardEvent::KeyType key_type, int code);
private:
  InputManagerImpl(const InputManagerImpl&);
  InputManagerImpl& operator=(const InputManagerImpl&);
};

#endif

/* EOF */
