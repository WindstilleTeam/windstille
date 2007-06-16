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

#include <assert.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "input/controller.hpp"
#include "display/display.hpp"
#include "screen_manager.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include "navigation/connection.hpp"
#include "navigation/segment.hpp"
#include "navigation_test.hpp"

NavigationTest::NavigationTest()
  : cursor(400, 300),
    graph(0),
    connection(0),
    selected_segment(0),
    selected_node(0),
    node_to_connect(0)
{
  graph = new NavigationGraph();

  Node* node1 = graph->add_node(Vector(100, 200));
  Node* node2 = graph->add_node(Vector(300, 400));
  //Node* node3 = graph->add_node(Vector(500, 300));
  //Node* node4 = graph->add_node(Vector(700, 400));

  graph->add_segment(node1, node2);
  //graph->add_segment(node2, node3);
  //graph->add_segment(node3, node4);
  
}

void
NavigationTest::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  graph->draw();

  Display::fill_rect(Rectf(cursor - Vector(2,2), Sizef(5,5)),  Color(1.0f, 1.0f, 1.0f));
  Display::draw_circle(cursor, 32.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  if (node_to_connect)
    {
      Display::fill_rect(Rectf(node_to_connect->get_pos() - Vector(2,2), Sizef(5,5)),  
                         Color(1.0f, 1.0f, 1.0f));
      Display::draw_line(node_to_connect->get_pos(), cursor, Color(1.0f, 1.0f, 1.0f, 0.5f));
    }

  if (selected_node)
    Display::draw_circle(selected_node->get_pos(), 12.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));

  if (selected_segment)
    Display::draw_line(selected_segment->get_line(), Color(1.0f, 1.0f, 1.0f, 1.0f));

  if (connection)
    connection->draw();
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
    {
      if (node_to_connect)
        {
          if (selected_node)
            graph->add_segment(node_to_connect, selected_node);
          
          node_to_connect = 0;
        }
      else if (selected_node)
        {
          node_to_connect = selected_node;
        }
      else if (selected_segment)
        {
          graph->split_segment(selected_segment);
          selected_segment = 0;
        }
      else
        {
          graph->add_node(cursor);
        }
    }

  if (controller.get_button_state(TERTIARY_BUTTON))
    {
      if (selected_node)
        {
          selected_node->set_pos(cursor);
        }
    }

  if (controller.button_was_pressed(SECONDARY_BUTTON))
    {
      if (selected_segment)
        {
          delete connection;
          connection = new Connection(selected_segment, 0.5f);
        }
    }

  if (connection)
    {
      Node* next_node;
      //float advance = 512.0f * controller.get_axis_state(X2_AXIS) * delta;

      // FIXME: xpad driver is buggy and reverses the Y2 axis
      Vector advance(512.0f * controller.get_axis_state(X2_AXIS) * delta,
                     -512.0f * controller.get_axis_state(Y2_AXIS) * delta);
      
      connection->advance(advance, next_node);
      
      if (!advance.is_null())
        {
          Segment* next_segment = 0;
          for(Node::Segments::iterator i = next_node->segments.begin(); i != next_node->segments.end(); ++i)
            {
              if (connection->get_segment() != *i)
                {
                  next_segment = *i;
                  break;
                }
            }
              
          if (!next_segment)
            {
              std::cout << "Dead End" << std::endl;
            }
          else
            {
              std::cout << "transition" << std::endl;
                           
              // FIXME: broken API this 0.0 or 1.0 depends on the segment
              if (connection->get_float_pos() == 0.0f)
                {
                  connection->set_pos(next_segment, 1.0f);
                }
              else if (connection->get_float_pos() == 1.0f)
                {
                  connection->set_pos(next_segment, 0.0f);
                }
              else
                {
                  assert(!"This should never been reached! - 3124");
                }
            }
        }
    }

  if (controller.button_was_pressed(PDA_BUTTON))
    {
      if (selected_node) {
        graph->remove_node(selected_node);
        selected_node = 0;
      } 
      
      if (selected_segment) {
        graph->remove_segment(selected_segment);
        selected_segment = 0;
      }      
    }

  selected_node = graph->find_closest_node(cursor, 32.0f);
  if (!selected_node)
    selected_segment = graph->find_closest_segment(cursor, 32.0f);
  else
    selected_segment = 0;
}

/* EOF */
