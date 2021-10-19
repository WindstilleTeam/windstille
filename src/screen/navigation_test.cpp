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

#include "screen/navigation_test.hpp"

#include <iostream>
#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <wstinput/controller.hpp>

#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include <surf/color.hpp>
#include <wstdisplay/graphics_context.hpp>
#include "navigation/edge.hpp"
#include "navigation/node.hpp"
#include "util/pathname.hpp"

NavigationTest::NavigationTest()
  : cursor(400, 300),
    stick(),
    player(200,200),
    old_player(),
    graph(new NavigationGraph()),
    connection(),
    selected_edge(),
    selected_node(),
    node_to_connect()
{
  try
  {
    ReaderDocument const& doc = ReaderDocument::from_file(Pathname("navigation.nav").get_sys_path());
    graph->load(doc.get_mapping());
  }
  catch(std::exception& err)
  {
    std::cout << "NavigationTest: " << err.what() << std::endl;
  }

  NodeHandle node1 = graph->add_node(glm::vec2(100, 200));
  NodeHandle node2 = graph->add_node(glm::vec2(300, 400));
  //Node* node3 = graph->add_node(glm::vec2(500, 300));
  //Node* node4 = graph->add_node(glm::vec2(700, 400));

  graph->add_edge(node1, node2);
  //graph->add_edge(node2, node3);
  //graph->add_edge(node3, node4);
}

void
NavigationTest::draw(wstdisplay::GraphicsContext& gc)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  graph->draw(gc);

  gc.fill_rect(geom::frect(cursor - glm::vec2(2,2), geom::fsize(5,5)),  surf::Color(1.0f, 1.0f, 1.0f));
  gc.draw_circle(cursor, 32.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));

  std::vector<NodeHandle> nodes = graph->find_nodes(cursor, 128.0f);
  for(std::vector<NodeHandle>::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    gc.draw_circle((*i)->get_pos(), 12.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  }

  if (node_to_connect)
  {
    gc.fill_rect(geom::frect(node_to_connect->get_pos() - glm::vec2(2,2), geom::fsize(5,5)),
                       surf::Color(1.0f, 1.0f, 1.0f));
    gc.draw_line(node_to_connect->get_pos(), cursor, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  }

  if (selected_node)
    gc.draw_circle(selected_node->get_pos(), 12.0f, surf::Color(1.0f, 1.0f, 1.0f, 1.0f));

  if (selected_edge)
    gc.draw_line(selected_edge->get_line(), surf::Color(1.0f, 1.0f, 1.0f, 1.0f));

  gc.fill_circle(player, 12.0f, surf::Color(0.0f, 0.0f, 1.0f, 1.0f));

  if (connection)
  {
    gc.fill_circle(connection->get_pos(), 16.0f, surf::Color(0.0f, 0.0f, 1.0f, 0.5f));
    gc.fill_circle(connection->get_pos(), 8.0f, surf::Color(0.0f, 1.0f, 1.0f));

    gc.draw_line(connection->get_pos(), connection->get_pos() + 100.0f*stick,
                       surf::Color(1.0f, 1.0f, 1.0f, 1.0f));
  }
}

void
NavigationTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
  {
    MenuManager::display_pause_menu();
  }

  cursor += glm::vec2(controller.get_axis_state(X_AXIS) * 500.0f * delta,
                     controller.get_axis_state(Y_AXIS) * 500.0f * delta);

  stick = glm::vec2(controller.get_axis_state(X2_AXIS),
                   controller.get_axis_state(Y2_AXIS));

  if (controller.button_was_pressed(PRIMARY_BUTTON))
  {
    if (node_to_connect)
    {
      if (selected_node)
        graph->add_edge(node_to_connect, selected_node);

      node_to_connect = nullptr;
    }
    else if (selected_node)
    {
      node_to_connect = selected_node;
    }
    else if (selected_edge)
    {
      graph->split_edge(selected_edge);
      selected_edge = nullptr;
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

    glm::vec2 advance = delta * 512.0f * stick;
    connection->advance(advance, next_node);

    player = connection->get_pos();

    if (!(advance.x == 0.0f && advance.y == 0.0f))
    { // Not all advancement got used up, which means we have hit
      // the end of a edge

      // FIXME: This should be a while loop, currently we are just
      // discarding the rest movement

      EdgePosition next_edge;
      float length = 0;
      for(Node::Edges::iterator i = next_node->edges.begin(); i != next_node->edges.end(); ++i)
      {
        if (connection->get_edge() != i->edge)
        { // Find out into the direction of which edge the stick is pointing
          glm::vec2 proj = glm::proj(stick, i->edge->get_vector());

          if (glm::length(proj) > length)
          {
            next_edge = *i;
            length       = glm::length(proj);
          }
        }
      }

      if (!next_edge.edge)
      {
        std::cout << "Dead End" << std::endl;
        connection.reset();

        // FIXME: Voodoo to fix connection/deadend cicles
        player += stick;
        old_player = player;
      }
      else
      {
        std::cout << "transition" << std::endl;
        *connection = next_edge;
      }
    }

    if (controller.get_button_state(AIM_BUTTON))
    {
      connection.reset();

      // FIXME: Voodoo to fix connection/dedaend cicles
      player += stick;
      old_player = player;
    }
  }
  else
  { // handle non connection based movement
    player += glm::vec2(0.0f, 100.0f) * delta;

    player.x += 512.0f * stick.x * delta;

    if (controller.get_button_state(AIM_BUTTON))
    {
      player.y -= 0.5f * 512.0f * delta;
    }

    std::vector<EdgePosition> positions = graph->find_intersections(geom::fline(old_player, player));
    if (!positions.empty())
    {
      std::cout << "Doing connection" << std::endl;
      connection.reset(new EdgePosition(positions.front()));
    }
  }

  if (controller.button_was_pressed(SELECT_BUTTON))
  {
    graph->save(std::cout);
  }

  if (controller.button_was_pressed(QUATERNARY_BUTTON))
  {
    if (selected_node) {
      graph->remove_node(selected_node);
      selected_node = nullptr;
    }

    if (selected_edge) {
      graph->remove_edge(selected_edge);
      selected_edge = nullptr;
    }
  }

  selected_node = graph->find_closest_node(cursor, 32.0f);
  if (!selected_node)
    selected_edge = graph->find_closest_edge(cursor, 32.0f);
  else
    selected_edge = nullptr;

  if (connection.get() && !graph->valid(connection->get_edge()))
  {
    connection.reset();
  }

  old_player = player;
}

/* EOF */
