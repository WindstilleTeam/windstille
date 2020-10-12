/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "screen/geometry_test.hpp"

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "app/console.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include "display/color.hpp"
#include "display/display.hpp"
#include "input/controller.hpp"

GeometryTest::GeometryTest()
  : line1(Vector2f(300, 300),
          Vector2f(500, 300)),
    line2(Vector2f(400, 200),
          Vector2f(400, 400)),
    cursor(line1.p1),
    cursor2(line1.p2),
    collision_point(),
    point_count(0),
    had_prev_collision(true)
{
}

void
GeometryTest::draw()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Display::draw_line(line1, Color(0.0f, 1.0f, 0.0f));
  Display::draw_line(line2, Color(0.0f, 1.0f, 0.0f));

  Display::fill_rect(Rectf(cursor - Vector2f(2,2), Sizef(5,5)),  Color(1.0f, 0.0f, 1.0f));
  Display::fill_rect(Rectf(cursor2 - Vector2f(2,2), Sizef(5,5)), Color(1.0f, 1.0f, 0.0f));

  Display::fill_rect(Rectf(collision_point - Vector2f(3,3), Sizef(7,7)), Color(1.0f, 1.0f, 1.0f));

  // Try vector projection
  Vector2f a(line1.p2 - line1.p1);
  Vector2f b(line2.p2 - line2.p1);
  Vector2f c(glm::proj(a, b));

  Display::draw_line(line1.p1, line1.p1 + c, Color(1.0f, 1.0f, 1.0f, 0.5f));

  int segments = std::max(0, int(cursor.y / 10));

  Display::fill_arc(Vector2f(200, 200), 100.0f, cursor.x, cursor2.x, Color(1.0f, 1.0f, 1.0f, 0.5f), segments);
  Display::draw_arc(Vector2f(200, 200), 100.0f, cursor.x, cursor2.x, Color(1.0f, 1.0f, 1.0f), segments);
  Display::draw_circle(Vector2f(200, 200), 128.0f, Color(1.0f, 1.0f, 1.0f), segments);
}

void
GeometryTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
  {
    MenuManager::display_pause_menu();
  }

  cursor.x += controller.get_axis_state(X_AXIS) * 500.0f * delta;
  cursor.y += controller.get_axis_state(Y_AXIS) * 500.0f * delta;

  cursor2.x += controller.get_axis_state(X2_AXIS) * 500.0f * delta;
  cursor2.y += controller.get_axis_state(Y2_AXIS) * 500.0f * delta;

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
      ConsoleLog << "Collision" << std::endl;
    had_prev_collision = true;
  }
  else
  {
    if (had_prev_collision)
      ConsoleLog << "No Collision" << std::endl;
    had_prev_collision = false;
    collision_point = Vector2f(32, 32);
  }
}

/* EOF */
