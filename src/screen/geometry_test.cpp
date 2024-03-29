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

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <wstinput/controller.hpp>

#include "app/console.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include <surf/color.hpp>
#include <wstdisplay/graphics_context.hpp>

namespace windstille {

GeometryTest::GeometryTest()
  : line1(glm::vec2(300, 300),
          glm::vec2(500, 300)),
    line2(glm::vec2(400, 200),
          glm::vec2(400, 400)),
    cursor(line1.p1.as_vec()),
    cursor2(line1.p2.as_vec()),
    collision_point(),
    point_count(0),
    had_prev_collision(true)
{
}

void
GeometryTest::draw(wstdisplay::GraphicsContext& gc)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gc.draw_line(line1, surf::Color(0.0f, 1.0f, 0.0f));
  gc.draw_line(line2, surf::Color(0.0f, 1.0f, 0.0f));

  gc.fill_rect(geom::frect(cursor - glm::vec2(2,2), geom::fsize(5,5)),  surf::Color(1.0f, 0.0f, 1.0f));
  gc.fill_rect(geom::frect(cursor2 - glm::vec2(2,2), geom::fsize(5,5)), surf::Color(1.0f, 1.0f, 0.0f));

  gc.fill_rect(geom::frect(collision_point.as_vec() - glm::vec2(3,3), geom::fsize(7,7)), surf::Color(1.0f, 1.0f, 1.0f));

  // Try vector projection
  glm::vec2 a(line1.p2.as_vec() - line1.p1.as_vec());
  glm::vec2 b(line2.p2.as_vec() - line2.p1.as_vec());
  glm::vec2 c(glm::proj(a, b));

  gc.draw_line(line1.p1, line1.p1.as_vec() + c, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));

  int segments = std::max(0, int(cursor.y / 10));

  gc.fill_arc(glm::vec2(200, 200), 100.0f, cursor.x, cursor2.x, surf::Color(1.0f, 1.0f, 1.0f, 0.5f), segments);
  gc.draw_arc(glm::vec2(200, 200), 100.0f, cursor.x, cursor2.x, surf::Color(1.0f, 1.0f, 1.0f), segments);
  gc.draw_circle(glm::vec2(200, 200), 128.0f, surf::Color(1.0f, 1.0f, 1.0f), segments);
}

void
GeometryTest::update(float delta, wstinput::Controller const& controller)
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
      cursor  = line2.p1.as_vec();
      cursor2 = line2.p2.as_vec();
    } else {
      cursor  = line1.p1.as_vec();
      cursor2 = line1.p2.as_vec();
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
    collision_point = glm::vec2(32, 32);
  }
}

} // namespace windstille

/* EOF */
