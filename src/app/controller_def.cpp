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

#include "controller_def.hpp"

WindstilleControllerDescription::WindstilleControllerDescription()
{
  // DPad
  add_button("menu-up-button",    MENU_UP_BUTTON);
  add_button("menu-down-button",  MENU_DOWN_BUTTON);
  add_button("menu-left-button",  MENU_LEFT_BUTTON);
  add_button("menu-right-button", MENU_RIGHT_BUTTON);

  // Face Button
  add_button("primary-button",    PRIMARY_BUTTON);
  add_button("secondary-button",  SECONDARY_BUTTON);
  add_button("tertiary-button",   TERTIARY_BUTTON);
  add_button("quaternary-button", QUATERNARY_BUTTON);

  // Stick Buttons
  add_button("left-stick-button",  LEFT_STICK_BUTTON);
  add_button("right-stick-button", RIGHT_STICK_BUTTON);

  // Shoulder Button
  add_button("left-shoulder-button",  LEFT_SHOULDER_BUTTON);
  add_button("right-shoulder-button", RIGHT_SHOULDER_BUTTON);

  // Back/Start
  add_button("select-button",    SELECT_BUTTON);
  add_button("start-button",     START_BUTTON);
  add_button("debug-button",     DEBUG_BUTTON);

  add_axis("left-trigger-axis",  LEFT_TRIGGER_AXIS);
  add_axis("right-trigger-axis", RIGHT_TRIGGER_AXIS);

  add_axis("x-axis", X_AXIS);
  add_axis("y-axis", Y_AXIS);

  add_axis("x2-axis", X2_AXIS);
  add_axis("y2-axis", Y2_AXIS);

  add_ball("mouse-motion-x", MOUSE_MOTION_X);
  add_ball("mouse-motion-y", MOUSE_MOTION_Y);
}

/* EOF */
