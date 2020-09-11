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

#ifndef HEADER_WINDSTILLE_APP_CONTROLLER_DEF_HPP
#define HEADER_WINDSTILLE_APP_CONTROLLER_DEF_HPP

#include "input/controller_description.hpp"

enum InputEventName
{
  MOUSE_MOTION_X,
  MOUSE_MOTION_Y,

  X_AXIS,
  Y_AXIS,

  X2_AXIS,
  Y2_AXIS,

  LEFT_STICK_BUTTON,
  RIGHT_STICK_BUTTON,

  PRIMARY_BUTTON,
  SECONDARY_BUTTON,
  TERTIARY_BUTTON,
  QUATERNARY_BUTTON,

  LEFT_SHOULDER_BUTTON,
  RIGHT_SHOULDER_BUTTON,

  LEFT_TRIGGER_AXIS,
  RIGHT_TRIGGER_AXIS,

  SELECT_BUTTON,
  START_BUTTON,
  DEBUG_BUTTON,

  // Keys for menu navigation, they come with hardcoded defaults for
  // the keyboard, a joystick might emmit them as well
  ESCAPE_BUTTON,
  ENTER_BUTTON,

  MENU_UP_BUTTON,
  MENU_DOWN_BUTTON,
  MENU_LEFT_BUTTON,
  MENU_RIGHT_BUTTON,

  LAST_EVENT
};

// Map the physical buttons to logical names
#define JUMP_BUTTON   PRIMARY_BUTTON
#define RUN_BUTTON    TERTIARY_BUTTON
#define USE_BUTTON    QUATERNARY_BUTTON
#define CRAWL_BUTTON  SECONDARY_BUTTON

#define OK_BUTTON     PRIMARY_BUTTON
#define CANCEL_BUTTON SECONDARY_BUTTON

#define VIEW_CENTER_BUTTON RIGHT_STICK_BUTTON
#define PAUSE_BUTTON       START_BUTTON
#define PDA_BUTTON         SELECT_BUTTON

#define INVENTORY_BUTTON   LEFT_SHOULDER_BUTTON
#define AIM_BUTTON         RIGHT_SHOULDER_BUTTON

ControllerDescription get_windstille_controller_description();

#endif

/* EOF */
