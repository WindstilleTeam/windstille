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

#include "editor/navgraph_insert_tool.hpp"

#include "editor/sector_model.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include "navigation/edge.hpp"
#include "editor/windstille_widget.hpp"
#include "editor/editor_window.hpp"
#include "editor/navigation_graph_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "editor/navgraph_edge_object_model.hpp"

NavgraphInsertTool::NavgraphInsertTool()
  : mouse_pos(),
    mode(NO_MODE),
    last_node(),
    connection_node(),
    mouse_over_node(),
    mouse_over_edge()
{
}

void
NavgraphInsertTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  mouse_pos = wst.get_state().screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));
  NavigationGraphModel& navgraph = wst.get_document().get_sector_model().get_nav_graph();

  // FIXME: Radius should scale with zoom
  boost::shared_ptr<NavGraphNodeObjectModel> node = navgraph.find_closest_node(mouse_pos, 16.0f); 
  boost::shared_ptr<NavGraphEdgeObjectModel> edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

  switch(mode)
  {
    case EDGE_MODE:
    {
      if (node)
      { // connect last node with existing node
        wst.get_document().navgraph_edge_add(wst.get_current_layer(), last_node, node);

        last_node.reset();
        connection_node.reset();
      }
      else
      { // connect last node with newly created node
        boost::shared_ptr<NavGraphNodeObjectModel> node_obj(new NavGraphNodeObjectModel(mouse_pos));
        
        wst.get_document().undo_group_begin();
        wst.get_document().navgraph_node_add(node_obj);
        wst.get_document().navgraph_edge_add(wst.get_current_layer(), last_node, node_obj);
        wst.get_document().undo_group_end();

        node = node_obj;
        last_node.reset();
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
        wst.get_document().navgraph_edge_remove(edge);
        
        edge.reset();
        mode = NO_MODE;
      }
      else
      {
        boost::shared_ptr<NavGraphNodeObjectModel> node_obj(new NavGraphNodeObjectModel(mouse_pos));//, sector));

        wst.get_document().navgraph_node_add(node_obj);

        last_node = node_obj;
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
  NavigationGraphModel& navgraph = wst.get_document().get_sector_model().get_nav_graph();
  mouse_pos = wst.get_state().screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));

  {
    // FIXME: Radius should scale with zoom
    boost::shared_ptr<NavGraphNodeObjectModel> new_mouse_over_node = navgraph.find_closest_node(mouse_pos, 16.0f);
    boost::shared_ptr<NavGraphEdgeObjectModel> new_mouse_over_edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

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
  mouse_pos = wst.get_state().screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));

  switch(mode)
  {
    case EDGE_MODE:
      connection_node = boost::shared_ptr<NavGraphNodeObjectModel>();
      wst.queue_draw();
      break;

    case NO_MODE:
      break;
  }
}

void
NavgraphInsertTool::mouse_right_down(GdkEventButton* /*event*/, WindstilleWidget& wst)
{
  NavigationGraphModel& navgraph = wst.get_document().get_sector_model().get_nav_graph();

  boost::shared_ptr<NavGraphNodeObjectModel> node = navgraph.find_closest_node(mouse_pos, 16.0f); // FIXME: Radius should scale with zoom
  boost::shared_ptr<NavGraphEdgeObjectModel> edge = navgraph.find_closest_edge(mouse_pos, 16.0f);

  if (node)
  {
    wst.get_document().navgraph_node_remove(node);

    mouse_over_edge.reset();
    mouse_over_node.reset();

    wst.queue_draw();
  }
  else if (edge)
  {
    wst.get_document().navgraph_edge_remove(edge);

    mouse_over_edge.reset();
    mouse_over_node.reset();

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
      sc.control().draw_line(last_node->get_world_pos(), connection_node->get_world_pos(), Color(1,1,1));
      sc.control().draw_rect(Rectf(connection_node->get_world_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));
    }
    else
    {
      sc.control().draw_line(last_node->get_world_pos(), mouse_pos, Color(1,1,1));
    }

    sc.control().draw_rect(Rectf(last_node->get_world_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));
  }

  if (mouse_over_node)
  {
    sc.control().draw_rect(Rectf(mouse_over_node->get_world_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));      
  }
  else if (mouse_over_edge)
  {
    sc.control().draw_line(mouse_over_edge->get_lhs()->get_world_pos(), 
                           mouse_over_edge->get_rhs()->get_world_pos(), Color(1,1,1));
  }
}
  
/* EOF */
