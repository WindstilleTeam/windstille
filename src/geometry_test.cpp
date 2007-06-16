/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <GL/glew.h>
#include <GL/gl.h>
#include "geometry_test.hpp"
#include "math/rect.hpp"
#include "console.hpp"
#include "math/size.hpp"
#include "controller_def.hpp"
#include "input/controller.hpp"
#include "screen_manager.hpp"
#include "display/display.hpp"

GeometryTest::GeometryTest()
{
  point_count = 0;
  had_prev_collision = true;

  line1 = Line(Vector(300, 300),
               Vector(500, 300));

  line2 = Line(Vector(400, 200),
               Vector(400, 400));

  cursor  = line1.p1;
  cursor2 = line1.p2;
}

void
GeometryTest::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Display::draw_line(line1, Color(0.0f, 1.0f, 0.0f));
  Display::draw_line(line2, Color(0.0f, 1.0f, 0.0f));

  Display::fill_rect(Rectf(cursor - Vector(2,2), Sizef(5,5)),  Color(1.0f, 0.0f, 1.0f));
  Display::fill_rect(Rectf(cursor2 - Vector(2,2), Sizef(5,5)), Color(1.0f, 1.0f, 0.0f));

  Display::fill_rect(Rectf(collision_point - Vector(3,3), Sizef(7,7)), Color(1.0f, 1.0f, 1.0f));

  // Try vector projection
  Vector a(line1.p2 - line1.p1);
  Vector b(line2.p2 - line2.p1);
  Vector c(a.project(b));
  
  Display::draw_line(line1.p1, line1.p1 + c, Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
GeometryTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
    {
      screen_manager.pop_screen();
    }

  cursor.x += controller.get_axis_state(X_AXIS) * 500.0f * delta;
  cursor.y += controller.get_axis_state(Y_AXIS) * 500.0f * delta;

  cursor2.x += controller.get_axis_state(X2_AXIS) * 500.0f * delta;
  cursor2.y -= controller.get_axis_state(Y2_AXIS) * 500.0f * delta;

  if (controller.button_was_pressed(PRIMARY_BUTTON))
    {
      if (point_count == 0) {
        cursor  = line2.p1;
        cursor2 = line2.p2;
      } else {
        cursor  = line1.p1;
        cursor2 = line1.p2;
      }

      point_count += 1;
      if (point_count > 1)
        point_count = 0;
    }


  if (point_count == 0)
    {
      line1.p1 = cursor;
      line1.p2 = cursor2;
    }
  else if (point_count == 1)
    {
      line2.p1 = cursor;
      line2.p2 = cursor2;
    }

  if (line1.intersect(line2, collision_point))
    {
      if (!had_prev_collision)
        console << "Collision" << std::endl;
      had_prev_collision = true;
    }
  else
    {
      if (had_prev_collision)
        console << "No Collision" << std::endl;
      had_prev_collision = false;
      collision_point = Vector(32, 32);
    }
}

/* EOF */
