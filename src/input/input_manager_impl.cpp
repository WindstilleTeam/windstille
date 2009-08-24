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

#include "input_manager_impl.hpp"

void
InputManagerImpl::add_axis_event(int name, float pos)
{
  InputEvent event;

  event.type = AXIS_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;

  controller.add_event(event);
  controller.set_axis_state(name, pos);
}

void
InputManagerImpl::add_ball_event  (int name, float pos)
{
  InputEvent event;

  event.type = BALL_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;

  controller.add_event(event);
  controller.set_ball_state(name, pos);  
}

void
InputManagerImpl::add_button_event(int name, bool down)
{
  InputEvent event;

  event.type = BUTTON_EVENT;
  event.button.name = name;
  event.button.down = down;

  controller.add_event(event);
  controller.set_button_state(name, down);
}

void
InputManagerImpl::add_keyboard_event(int , KeyboardEvent::KeyType key_type, int code)
{
  InputEvent event;
  event.type = KEYBOARD_EVENT;
  event.keyboard.key_type = key_type;
  event.keyboard.code     = code;

  controller.add_event(event);
}

void
InputManagerImpl::clear()
{
  controller.clear();
}

const Controller&
InputManagerImpl::get_controller() const
{
  return controller;
}

/* EOF */
