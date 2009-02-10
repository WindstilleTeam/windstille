/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "sexpr_file_reader.hpp"
#include "input/controller.hpp"
#include "display/display.hpp"
#include "screen_manager.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include "navigation/segment_position.hpp"
#include "navigation/segment.hpp"
#include "navigation_test.hpp"

NavigationTest::NavigationTest()
  : cursor(400, 300),
    player(200,200),
    graph(0),
    connection(0),
    selected_segment(0),
    selected_node(0),
    node_to_connect(0)
{
  graph = new NavigationGraph();

  try {
    FileReader reader = FileReader::parse("navigation.nav");
    graph->load(reader);
  } catch(std::exception& err) {
    std::cout << "NavigationTest: " << err.what() << std::endl;
  }

  NodeHandle node1 = graph->add_node(Vector(100, 200));
  NodeHandle node2 = graph->add_node(Vector(300, 400));
  //Node* node3 = graph->add_node(Vector(500, 300));
  //Node* node4 = graph->add_node(Vector(700, 400));

  graph->add_segment(node1, node2);
  //graph->add_segment(node2, node3);
  //graph->add_segment(node3, node4);
  
}

void
NavigationTest::draw()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  graph->draw();

  Display::fill_rect(Rectf(cursor - Vector(2,2), Sizef(5,5)),  Color(1.0f, 1.0f, 1.0f));
  Display::draw_circle(cursor, 32.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  std::vector<NodeHandle> nodes = graph->find_nodes(cursor, 128.0f);
  for(std::vector<NodeHandle>::iterator i = nodes.begin(); i != nodes.end(); ++i)
    {
      Display::draw_circle((*i)->get_pos(), 12.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
    }

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

  Display::fill_circle(player, 12.0f, Color(0.0f, 0.0f, 1.0f, 1.0f));

  if (connection)
    {
      Display::fill_circle(connection->get_pos(), 16.0f, Color(0.0f, 0.0f, 1.0f, 0.5f));
      Display::fill_circle(connection->get_pos(), 8.0f, Color(0.0f, 1.0f, 1.0f));
     
      Display::draw_line(connection->get_pos(), connection->get_pos() + 100.0f*stick,
                         Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

void
NavigationTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
    {
      screen_manager.pop_screen();
    }

  cursor += Vector(controller.get_axis_state(X_AXIS) * 500.0f * delta,
                   controller.get_axis_state(Y_AXIS) * 500.0f * delta);

  stick = Vector(controller.get_axis_state(X2_AXIS),
                 controller.get_axis_state(Y2_AXIS));

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
    { // Move node
      if (selected_node)
        {
          selected_node->set_pos(cursor);
        }
    }

  if (controller.button_was_pressed(SECONDARY_BUTTON))
    { // Set player
      player = old_player = cursor;
    }

  if (connection)
    { 
      // Handle the movement of the connection
      Node* next_node;
      //float advance = 512.0f * controller.get_axis_state(X2_AXIS) * delta;
      
      Vector advance = delta * 512.0f * stick;
      connection->advance(advance, next_node);

      player = connection->get_pos();
      
      if (!advance.is_null())
        { // Not all advancement got used up, which means we have hit
          // the end of a segment

          // FIXME: This should be a while loop, currently we are just
          // discarding the rest movement

          SegmentPosition next_segment;
          float length = 0;
          for(Node::Segments::iterator i = next_node->segments.begin(); i != next_node->segments.end(); ++i)
            {
              if (connection->get_segment() != i->segment)
                { // Find out into the direction of which segment the stick is pointing
                  Vector proj = stick.project(i->segment->get_vector());
                  
                  if (proj.length() > length)
                    {
                      next_segment = *i;
                      length       = proj.length();
                    }
                }
            }
              
          if (!next_segment.segment)
            {
              std::cout << "Dead End" << std::endl;
              delete connection;
              connection = 0;

              // FIXME: Voodoo to fix connection/deadend cicles
              player += stick;
              old_player = player;
            }
          else
            {
              std::cout << "transition" << std::endl;
              *connection = next_segment;
            }
        }

      if (controller.get_button_state(AIM_BUTTON))
        {         
          delete connection;
          connection = 0;

          // FIXME: Voodoo to fix connection/dedaend cicles
          player += stick;
          old_player = player;
        }
    }
  else
    { // handle non connection based movement
      player += Vector(0.0f, 100.0f) * delta;
      
      player.x += 512.0f * stick.x * delta;

      if (controller.get_button_state(AIM_BUTTON))
        {
          player.y -= 0.5f * 512.0f * delta;
        }

      std::vector<SegmentPosition> positions = graph->find_intersections(Line(old_player, player));
      if (!positions.empty()) {
        std::cout << "Doing connection" << std::endl;
        connection = new SegmentPosition(positions.front());
      }
    }
  
  if (controller.button_was_pressed(VIEW_CENTER_BUTTON))
    {
      graph->save(std::cout);
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

  if (connection && !graph->valid(connection->get_segment()))
    {
      delete connection;
      connection = 0;
    }

  old_player = player;
}

/* EOF */
