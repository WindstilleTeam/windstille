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

#include <gtkmm/menu.h>

#include "display/scene_context.hpp"
#include "sector_model.hpp"
#include "windstille_widget.hpp"
#include "editor_window.hpp"
#include "select_tool.hpp"

static const uint32_t MOVE_TIMEOUT = 100;
static const int MOVE_THRESHOLD = 16.0f;

SelectTool::SelectTool()
  : mode(NO_MODE)
    
{
}

void
SelectTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  start_time = event->time;
  click_pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  ctrl_point = wst.get_control_point(click_pos);
  if (ctrl_point)
    {
      mode = CONTROL_DRAG_MODE;
      wst.clear_control_points();
      ctrl_point->on_move_start(event);
    }
  else
    {  
      ObjectModelHandle object = wst.get_sector_model()->get_object_at(click_pos, wst.get_select_mask());
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
      
          mode = OBJECT_DRAG_MODE;
        }
      else
        {
          rect.left   = click_pos.x;
          rect.top    = click_pos.y;
          rect.right  = click_pos.x;
          rect.bottom = click_pos.y;
      
          mode = SELECT_MODE;
        }
    }

  wst.queue_draw();
}

Vector2f
SelectTool::process_snap(WindstilleWidget& wst)
{
  // ignore all objects in the selection
  std::set<ObjectModelHandle> ignore_objects(selection->begin(), selection->end());

  if (!wst.get_draw_only_active_layer())
    {
      // ignore all objects not on the current active layer
      for(Layer::iterator i = wst.get_current_layer()->begin(); i != wst.get_current_layer()->end(); ++i)
        { // FIXME: Should iterate over all objects, not just objects in the current layer
          if (!wst.get_select_mask().match((*i)->get_select_mask()))
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

void
SelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  if (mode == CONTROL_DRAG_MODE)
    {
      ctrl_point->on_move_update(event, pos - click_pos);
      wst.queue_draw();
    }
  else if (mode == OBJECT_DRAG_MODE)
    {
      Vector2f offset = pos - click_pos;
          
      if ((event->time - start_time) > MOVE_TIMEOUT ||
          offset.length() > MOVE_THRESHOLD)
        {
          if (!selection->is_moving())
            selection->on_move_start();

          selection->on_move_update(offset);
      
          if (event->state & GDK_CONTROL_MASK)
            {
              selection->on_move_update(offset + process_snap(wst));
            }

          wst.queue_draw();
        }
    }
  else if (mode == SELECT_MODE)
    {
      rect.left   = click_pos.x;
      rect.top    = click_pos.y;
      rect.right  = pos.x;
      rect.bottom = pos.y;

      wst.queue_draw();
    }
}

void
SelectTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  Vector2f pos = wst.get_state().screen_to_world(Vector2f(event->x, event->y));

  // Select objects
  if (mode == CONTROL_DRAG_MODE)
    {
      ctrl_point->on_move_end(event, pos - click_pos);
      wst.create_control_points();
      wst.queue_draw();
    }
  else if (mode == OBJECT_DRAG_MODE)
    {
      Vector2f offset = pos - click_pos;

      if (event->time - start_time > MOVE_TIMEOUT ||
          offset.length() > MOVE_THRESHOLD)
        {
          selection->on_move_update(offset);
      
          if (event->state & GDK_CONTROL_MASK)
            {
              selection->on_move_end(wst, offset + process_snap(wst));
            }
          else
            {
              selection->on_move_end(wst, offset);
            }
          wst.queue_draw();
        }
    }
  else if (mode == SELECT_MODE)
    {
      mode = NO_MODE;
      rect.normalize();
      if (event->state & GDK_SHIFT_MASK)
        {
          SelectionHandle new_selection = wst.get_sector_model()->get_selection(rect, wst.get_select_mask());
          wst.get_selection()->add(new_selection->begin(), new_selection->end());
        }
      else
        {
          wst.set_selection(wst.get_sector_model()->get_selection(rect, wst.get_select_mask()));
        }
      wst.queue_draw();
    }

  mode = NO_MODE;
}

void
SelectTool::mouse_right_down(GdkEventButton* event, WindstilleWidget& wst)
{
  Gtk::Menu* menu = static_cast<Gtk::Menu*>(EditorWindow::current()->get_ui_manager()->get_widget("/PopupMenu"));
  menu->popup(event->button, event->time);
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
