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

#include "app/controller_def.hpp"

wstinput::ControllerDescription get_windstille_controller_description()
{
  wstinput::ControllerDescription desc;

  // DPad
  desc.add_button("menu-up-button",    MENU_UP_BUTTON);
  desc.add_button("menu-down-button",  MENU_DOWN_BUTTON);
  desc.add_button("menu-left-button",  MENU_LEFT_BUTTON);
  desc.add_button("menu-right-button", MENU_RIGHT_BUTTON);

  // Face Button
  desc.add_button("primary-button",    PRIMARY_BUTTON);
  desc.add_button("secondary-button",  SECONDARY_BUTTON);
  desc.add_button("tertiary-button",   TERTIARY_BUTTON);
  desc.add_button("quaternary-button", QUATERNARY_BUTTON);

  // Stick Buttons
  desc.add_button("left-stick-button",  LEFT_STICK_BUTTON);
  desc.add_button("right-stick-button", RIGHT_STICK_BUTTON);

  // Shoulder Button
  desc.add_button("left-shoulder-button",  LEFT_SHOULDER_BUTTON);
  desc.add_button("right-shoulder-button", RIGHT_SHOULDER_BUTTON);

  // Back/Start
  desc.add_button("select-button",    SELECT_BUTTON);
  desc.add_button("start-button",     START_BUTTON);
  desc.add_button("debug-button",     DEBUG_BUTTON);

  desc.add_axis("left-trigger-axis",  LEFT_TRIGGER_AXIS);
  desc.add_axis("right-trigger-axis", RIGHT_TRIGGER_AXIS);

  desc.add_axis("x-axis", X_AXIS);
  desc.add_axis("y-axis", Y_AXIS);

  desc.add_axis("x2-axis", X2_AXIS);
  desc.add_axis("y2-axis", Y2_AXIS);

  desc.add_ball("mouse-motion-x", MOUSE_MOTION_X);
  desc.add_ball("mouse-motion-y", MOUSE_MOTION_Y);

  return desc;
}

/* EOF */
