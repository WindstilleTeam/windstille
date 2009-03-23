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

#include "display/scene_context.hpp"
#include "sector_model.hpp"
#include "windstille_widget.hpp"
#include "select_tool.hpp"

SelectTool::SelectTool()
  : mode(NO_MODE)
    
{
}

bool
SelectTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  click_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));
  
  ObjectModelHandle object = wst.get_sector_model()->get_object_at(click_pos);
  if (object.get())
    {
      if (wst.get_selection()->has_object(object))
        {
          selection = wst.get_selection();
        }
      else
        {
          selection = Selection::create();
          selection->add(object);
          wst.set_selection(selection);
        }
      
      mode = DRAG_MODE;
      selection->on_move_start();
    }
  else
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
SelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  if (mode == DRAG_MODE)
    {
      selection->on_move_update(pos - click_pos);
    }
  else if (mode == SELECT_MODE)
    {
      rect.left   = click_pos.x;
      rect.top    = click_pos.y;
      rect.right  = pos.x;
      rect.bottom = pos.y;
    }
  
  return true;
}

bool
SelectTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  // Select objects
  if (mode == DRAG_MODE)
    {
      selection->on_move_end(pos - click_pos);
    }
  else if (mode == SELECT_MODE)
    {
      mode = NO_MODE;
      rect.normalize();
      wst.set_selection(wst.get_sector_model()->get_selection(rect));
    }

  mode = NO_MODE;

  return true;
}

void
SelectTool::draw(SceneContext& sc)
{
  if (mode == SELECT_MODE)
    {
      sc.control().fill_rect(rect, Color(0.5f, 0.5f, 1.0f, 0.25));
      sc.control().draw_rect(rect, Color(0.5f, 0.5f, 1.0f)); 
    }
}

/* EOF */
