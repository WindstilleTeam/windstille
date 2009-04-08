/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "sector_model.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include "navigation/edge.hpp"
#include "windstille_widget.hpp"
#include "editor_window.hpp"

#include "navgraph_insert_tool.hpp"

NavgraphInsertTool::NavgraphInsertTool()
  : mode(NO_MODE)
{
}

void
NavgraphInsertTool::mouse_down (GdkEventButton* event, WindstilleWidget& wst)
{
  mouse_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));
  NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();

  NodeHandle node = navgraph.find_closest_node(mouse_pos, 16.0f); // FIXME: Radius should scale with zoom
  EdgeHandle edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

  switch(mode)
    {
      case EDGE_MODE:
        {
          if (node)
            { // connect last node with existing node
              navgraph.add_edge(last_node, node);
              last_node = NodeHandle();
              connection_node = NodeHandle();
            }
          else
            { // connect last node with newly created node
              node = navgraph.add_node(mouse_pos);
              navgraph.add_edge(last_node, node);
              last_node = NodeHandle();
            }
          mode = NO_MODE;        
        }
        break;

      case NO_MODE:
        {
          if (node)
            {
              last_node = node;
              mode = EDGE_MODE;
            }
          else if (edge)
            {
              navgraph.remove_edge(edge);
              mode = NO_MODE;
            }
          else
            {
              last_node = navgraph.add_node(mouse_pos);
              mode = EDGE_MODE;
            }
        }
        break;
    }

  wst.queue_draw();
}

void
NavgraphInsertTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();
  mouse_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  {
    NodeHandle new_mouse_over_node = navgraph.find_closest_node(mouse_pos, 16.0f); // FIXME: Radius should scale with zoom
    EdgeHandle new_mouse_over_edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

    if (new_mouse_over_node != mouse_over_node ||
        new_mouse_over_edge != mouse_over_edge)
      {
        mouse_over_node = new_mouse_over_node; 
        mouse_over_edge = new_mouse_over_edge; 

        wst.queue_draw();
      }
  }

  switch(mode)
    {
      case EDGE_MODE:
        connection_node = navgraph.find_closest_node(mouse_pos, 16.0f); // FIXME: Radius should scale with zoom
        wst.queue_draw();
        break;

      case NO_MODE:
        break;
    }
}

void
NavgraphInsertTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  mouse_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));
  //NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();

  switch(mode)
    {
      case EDGE_MODE:
        connection_node = NodeHandle();
        wst.queue_draw();
        break;

      case NO_MODE:
        break;
    }
}

void
NavgraphInsertTool::mouse_right_down(GdkEventButton* event, WindstilleWidget& wst)
{
  NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();

  NodeHandle node = navgraph.find_closest_node(mouse_pos, 16.0f); // FIXME: Radius should scale with zoom
  EdgeHandle edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

  if (node)
    {
      navgraph.remove_node(node);

      mouse_over_edge = EdgeHandle();
      mouse_over_node = NodeHandle();

      wst.queue_draw();
    }
  else if (edge)
    {
      navgraph.remove_edge(edge);

      mouse_over_edge = EdgeHandle();
      mouse_over_node = NodeHandle();

      wst.queue_draw();
    }
  else
    {
      
    }
}  
  
void
NavgraphInsertTool::draw(SceneContext& sc)
{
  if (last_node)
    {
      if (connection_node)
        {
          sc.control().draw_line(last_node->get_pos(), connection_node->get_pos(), Color(1,1,1));
          sc.control().draw_rect(Rectf(connection_node->get_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));
        }
      else
        {
          sc.control().draw_line(last_node->get_pos(), mouse_pos, Color(1,1,1));
        }

      sc.control().draw_rect(Rectf(last_node->get_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));
    }

  if (mouse_over_node)
    {
      sc.control().draw_rect(Rectf(mouse_over_node->get_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));      
    }
  else if (mouse_over_edge)
    {
      sc.control().draw_line(mouse_over_edge->get_node1()->get_pos(), 
                             mouse_over_edge->get_node2()->get_pos(), Color(1,1,1));
    }
}
  
/* EOF */
