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
#include "windstille_widget.hpp"

#include "navgraph_select_tool.hpp"

NavgraphSelectTool::NavgraphSelectTool()
{
}

NavgraphSelectTool::~NavgraphSelectTool()
{
}

void
NavgraphSelectTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  click_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));
  NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();

  NodeHandle node = navgraph.find_closest_node(click_pos, 16.0f); // FIXME: Radius should scale with zoom
  EdgeHandle edge = navgraph.find_closest_edge(click_pos, 16.0f);

  if (node)
    {
      mode = DRAG_MODE;
      selection.clear();
      selection.insert(node);
    }
  else if (edge)
    {
      navgraph.remove_edge(edge);
      mode = NO_MODE;
    }
  else
    {
      rect.left   = click_pos.x;
      rect.top    = click_pos.y;
      rect.right  = click_pos.x;
      rect.bottom = click_pos.y;
      
      selection.clear();
      mode = SELECT_MODE;
    }
}

void
NavgraphSelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  switch(mode)
    {
      case DRAG_MODE:
        {
          NodeHandle node = *selection.begin();
          node->set_pos(pos);
        }
        break;

      case SELECT_MODE:
        rect.right  = pos.x;
        rect.bottom = pos.y;
        break;

      case NO_MODE:
        break;
    }
}

void
NavgraphSelectTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));
  NavigationGraph& navgraph = *wst.get_sector_model()->get_nav_graph();

  switch(mode)
    {
      case DRAG_MODE:
        break;

      case SELECT_MODE:
        {
          rect.normalize();
          const std::vector<NodeHandle>& lst = navgraph.find_nodes(rect);
          selection.insert(lst.begin(), lst.end());
          std::cout << "NavgraphInsertTool::select: " << selection.size() << std::endl;
          wst.queue_draw();
        }
        break;

      case NO_MODE:
        break;
    }

  mode = NO_MODE;
}

void
NavgraphSelectTool::draw(SceneContext& sc)
{
  if (mode == SELECT_MODE)
    {
      sc.control().fill_rect(rect, Color(1.0f, 0.5f, 0.5f, 0.25));
      sc.control().draw_rect(rect, Color(1.0f, 0.5f, 0.5f)); 
    }

  std::cout << "NavgraphInsertTool::draw: " << selection.size() << std::endl;
  for(std::set<NodeHandle>::const_iterator i = selection.begin(); i != selection.end(); ++i)
    {
      sc.control().draw_rect(Rectf((*i)->get_pos() - Vector2f(16,16), Sizef(32,32)), Color(1.0f, 1.0f, 1.0f));
    }
}

/* EOF */
