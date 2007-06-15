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
#include "input/controller.hpp"
#include "display/display.hpp"
#include "screen_manager.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include "navigation/segment.hpp"
#include "navigation_test.hpp"

NavigationTest::NavigationTest()
  : cursor(400, 300)
{
  graph = new NavigationGraph();

  Node* node1 = graph->add_node(Vector(100, 200));
  Node* node2 = graph->add_node(Vector(300, 400));
  Node* node3 = graph->add_node(Vector(500, 300));
  Node* node4 = graph->add_node(Vector(700, 400));

  graph->add_segment(node1, node2);
  graph->add_segment(node2, node3);
  graph->add_segment(node3, node4);
  
}

void
NavigationTest::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  graph->draw();

  Display::fill_rect(Rectf(cursor - Vector(2,2), Sizef(5,5)),  Color(1.0f, 1.0f, 1.0f));
  Display::draw_circle(cursor, 32.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  Node* node = graph->find_closest_node(cursor, 32.0f);
  if (node)
    Display::draw_circle(node->get_pos(), 12.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));

  Segment* segment = graph->find_closest_segment(cursor, 32.0f);
  if (segment)
    Display::draw_line(segment->get_line(), Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void
NavigationTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
    {
      screen_manager.pop_screen();
    }

  cursor.x += controller.get_axis_state(X_AXIS) * 500.0f * delta;
  cursor.y += controller.get_axis_state(Y_AXIS) * 500.0f * delta;

  if (controller.button_was_pressed(PRIMARY_BUTTON))
    graph->add_node(cursor);
}

/* EOF */
