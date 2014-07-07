//  $Id: gamepad_controller.cxx,v 1.3 2003/08/12 08:24:41 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "gamepad_controller.hxx"

#include <iostream>

GamepadController::GamepadController (unsigned int joy_num)
{
#if 0
  if (CL_Input::joysticks.size () > joy_num)
    {
      std::cout << "Joystick found" << std::endl;
      y_axis = CL_Input::joysticks[joy_num]->get_axis (1);
      x_axis = CL_Input::joysticks[joy_num]->get_axis (0);
      
      surround_button  = CL_Input::joysticks[joy_num]->get_button (6);
      smartbomb_button = CL_Input::joysticks[joy_num]->get_button (3);
      fire_button      = CL_Input::joysticks[joy_num]->get_button (4);
      jump_button      = CL_Input::joysticks[joy_num]->get_button (1);
    }
  else
    {
      std::cout << "No Joystick found" << std::endl;
      exit (1);
    }
#endif
}

// Directional Pad
bool 
GamepadController::is_right ()
{
#if 0
  return (x_axis->get_pos () > 0.5);
#else
  return false;
#endif
}

bool
GamepadController::is_left ()
{
#if 0
  return (x_axis->get_pos () < -0.5);
#else
  return false;
#endif
}

bool
GamepadController::is_up ()
{
#if 0
  return (y_axis->get_pos () < -0.5);
#else
  return false;
#endif
}

bool 
GamepadController::is_down ()
{
#if 0
  return (y_axis->get_pos () > 0.5);
#else
  return false;
#endif
}

// Buttons
bool
GamepadController::fire_pressed ()
{
#if 0
  return fire_button->is_pressed ();
#else
  return false;
#endif
}

bool 
GamepadController::jump_pressed ()
{
#if 0
  return jump_button->is_pressed ();
#else
  return false;
#endif
}

bool 
GamepadController::surround_pressed ()
{
#if 0
  return surround_button->is_pressed ();
#else
  return false;
#endif
}

bool 
GamepadController::smartbomb_pressed ()
{
#if 0
  return smartbomb_button->is_pressed ();
#else
  return false;
#endif
}

/* EOF */
