/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP

#include "input/controller.hpp"
#include "input/input_event.hpp"
#include "util/currenton.hpp"

class InputManager
{
protected:
  ControllerDescription controller_description;
  Controller controller;

public:
  InputManager(const ControllerDescription& controller_description_)
    : controller_description(controller_description_),
      controller()
  {}
  virtual ~InputManager() {}

  const ControllerDescription& get_controller_description() const { return controller_description; }
  const Controller& get_controller() const;
  void clear();

  virtual void add_axis_event  (int name, float pos);
  virtual void add_ball_event  (int name, float pos);
  virtual void add_button_event(int name, bool down);
  virtual void add_keyboard_event(int name, KeyboardEvent::KeyType key_type, int code);

private:
  InputManager(const InputManager&);
  InputManager& operator=(const InputManager&);
};

#endif

/* EOF */
