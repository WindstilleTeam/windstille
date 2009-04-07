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

#include "windstille_widget.hpp"
#include "navgraph_tool.hpp"

NavgraphTool::NavgraphTool()
{
}

bool
NavgraphTool::mouse_down (GdkEventButton* event, WindstilleWidget& wst)
{
  click_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  {
    rect.left   = click_pos.x;
    rect.top    = click_pos.y;
    rect.right  = click_pos.x;
    rect.bottom = click_pos.y;
      
    mode = SELECT_MODE;
  }

  return true;
}

bool
NavgraphTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  switch(mode)
    {
      case DRAG_MODE:
        break;

      case SELECT_MODE:
        rect.right  = pos.x;
        rect.bottom = pos.y;
        break;

      case NO_MODE:
        break;
    }

  return true;
}

bool
NavgraphTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  mode = NO_MODE;

  return true;
}
  
void
NavgraphTool::draw(SceneContext& sc)
{
  if (mode == SELECT_MODE)
    {
      sc.control().fill_rect(rect, Color(1.0f, 0.5f, 0.5f, 0.25));
      sc.control().draw_rect(rect, Color(1.0f, 0.5f, 0.5f)); 
    }
}
  
/* EOF */
