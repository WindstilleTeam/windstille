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

#ifndef HEADER_WINDSTILLE_CONTROLLER_DEF_HPP
#define HEADER_WINDSTILLE_CONTROLLER_DEF_HPP

#include <map>
#include <string>
#include "input/input_event.hpp"
#include "input/controller_description.hpp"

enum InputEventName
  { 
    MOUSE_MOTION_X, 
    MOUSE_MOTION_Y, 

    X_AXIS, // used to run left/right
    Y_AXIS,  // used to aim up/down

    X2_AXIS,
    Y2_AXIS, 
    
    VIEW_CENTER_BUTTON,

    PRIMARY_BUTTON,   // used to ok a dialog or for running
    SECONDARY_BUTTON, // used to cancel a dialog or for jumping
    TERTIARY_BUTTON,  // used to cancel a dialog or for jumping
    PDA_BUTTON,       // display the PDA
    INVENTORY_BUTTON, // display inventory button
    
    PAUSE_BUTTON,     // used to pause the game
    AIM_BUTTON,       // used to draw the gun and aim

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

#define JUMP_BUTTON   PRIMARY_BUTTON
#define RUN_BUTTON    TERTIARY_BUTTON
#define USE_BUTTON    SECONDARY_BUTTON
#define OK_BUTTON     PRIMARY_BUTTON
#define CANCEL_BUTTON SECONDARY_BUTTON

extern ControllerDescription controller_description;

#endif

/* EOF */
