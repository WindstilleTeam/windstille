//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include "physics.hxx"

int main()
{
  CL_SetupCore    core_setup;
  CL_SetupGL      gl_setup;
  CL_SetupDisplay display_setup;

  // Init Display
  CL_DisplayWindow window("Physic Test", 800, 600);
  
  // Init Physic Engine
  Physics physics;

  bool create_object = false;
  int x_pos;
  int y_pos;

  physics.add_object(PhysicObject(0, 0, 32, 600)).movable = false;
  physics.add_object(PhysicObject(800-32, 0, 32, 600)).movable = false;

  physics.add_object(PhysicObject(32, 0, 800-32, 32)).movable = false;
  physics.add_object(PhysicObject(32, 600-32, 800-32, 32)).movable = false;

  physics.add_object(PhysicObject(400, 300, 100, 100)).movable = false;

  // Run Main Loop
  while(!CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    {
      CL_Display::clear();

      physics.update(0.033f);
      physics.draw();

      CL_Display::flip();

      CL_System::keep_alive();

      if (CL_Mouse::get_keycode(CL_MOUSE_LEFT) && !create_object)
        {
          create_object = true;
          x_pos = CL_Mouse::get_x();
          y_pos = CL_Mouse::get_y();
        }
      else if (create_object && !CL_Mouse::get_keycode(CL_MOUSE_LEFT))
        {
          create_object = false;

          int n_x_pos = CL_Mouse::get_x();
          int n_y_pos = CL_Mouse::get_y();

          PhysicObject& obj = physics.add_object(PhysicObject(x_pos, y_pos, 32, 32));
          obj.x_velocity = x_pos - n_x_pos;
          obj.y_velocity = y_pos - n_y_pos;
        }
    }

  // Deinit Physic Engine

  // Deinit Display
}

/* EOF */
