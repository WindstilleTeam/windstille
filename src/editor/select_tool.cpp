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
  
  ObjectModelHandle object = wst.get_sector_model()->get_object_at(click_pos, wst.get_layer_mask());
  if (object.get())
    {
      if (wst.get_selection()->has_object(object))
        {
          selection = wst.get_selection();
          if (event->state & GDK_SHIFT_MASK)
            {
              selection->remove(object);
            }
          else
            {
              selection = wst.get_selection();
            }
        }
      else
        {
          if (event->state & GDK_SHIFT_MASK)
            {
              selection = wst.get_selection();
              selection->add(object);
            }
          else
            {
              selection = Selection::create();
              selection->add(object);
              wst.set_selection(selection);
            }
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

Vector2f
SelectTool::process_snap(WindstilleWidget& wst)
{
  // ignore all objects in the selection
  std::set<ObjectModelHandle> ignore_objects(selection->begin(), selection->end());

  if (!wst.get_draw_only_active_layer())
    {
      // ignore all objects not on the current active layer
      for(HardLayer::iterator i = wst.get_current_layer()->begin(); i != wst.get_current_layer()->end(); ++i)
        { // FIXME: Should iterate over all objects, not just objects in the current layer
          if (!wst.get_layer_mask().match((*i)->get_layers()))
            ignore_objects.insert(*i);
        }
    }

  SnapData best_snap;

  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      SnapData snap = wst.get_sector_model()->snap_object((*i)->get_bounding_box(), ignore_objects);
      best_snap.merge(snap);
    }
              
  return best_snap.offset;
}

bool
SelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  if (mode == DRAG_MODE)
    {
      selection->on_move_update(pos - click_pos);
      
      if (event->state & GDK_CONTROL_MASK)
        {
          selection->on_move_update(pos - click_pos + process_snap(wst));
        }
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
      selection->on_move_update(pos - click_pos);
      
      if (event->state & GDK_CONTROL_MASK)
        {
          selection->on_move_end(pos - click_pos + process_snap(wst));
        }
      else
        {
          selection->on_move_end(pos - click_pos);
        }
    }
  else if (mode == SELECT_MODE)
    {
      mode = NO_MODE;
      rect.normalize();
      if (event->state & GDK_SHIFT_MASK)
        {
          SelectionHandle new_selection = wst.get_sector_model()->get_selection(rect, wst.get_layer_mask());
          wst.get_selection()->add(new_selection->begin(), new_selection->end());
        }
      else
        {
          wst.set_selection(wst.get_sector_model()->get_selection(rect, wst.get_layer_mask()));
        }
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
