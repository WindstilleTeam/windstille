/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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
#include "editor/sector_model.hpp"
#include "editor/windstille_widget.hpp"
#include "editor/editor_window.hpp"
#include "editor/select_tool.hpp"

static const guint32 MOVE_TIMEOUT = 100;
static const int MOVE_THRESHOLD = 16;

SelectTool::SelectTool() :
  click_pos(),
  rect(),
  selection(),
  ctrl_point(),
  start_time(),
  mode(NO_MODE)
{
}

void
SelectTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  start_time = event->time;
  click_pos = wst.get_state().screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y)));

  ctrl_point = wst.get_document().get_control_point(click_pos);
  if (ctrl_point)
  {
    mode = CONTROL_DRAG_MODE;
    wst.get_document().clear_control_points();
    ctrl_point->on_move_start(event);
  }
  else
  {
    ObjectModelHandle object = wst.get_document().get_sector_model().get_object_at(click_pos, wst.get_select_mask());
    if (object.get())
    {
      if (wst.get_document().get_selection()->has_object(object))
      {
        selection = wst.get_document().get_selection();
        if (event->state & GDK_SHIFT_MASK)
        {
          selection->remove(object);
        }
        else
        {
          selection = wst.get_document().get_selection();
        }
      }
      else
      {
        if (event->state & GDK_SHIFT_MASK)
        {
          selection = wst.get_document().get_selection();
          selection->add(object);
        }
        else
        {
          selection = Selection::create();
          selection->add(object);
          wst.get_document().set_selection(selection);
        }
      }

      mode = OBJECT_DRAG_MODE;
    }
    else
    {
      rect = Rectf(click_pos, click_pos);

      mode = SELECT_MODE;
    }
  }

  wst.queue_draw();
}

glm::vec2
SelectTool::process_grid_snap(WindstilleWidget& wst)
{
  SnapData best_snap;

  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
  {
    best_snap.merge((*i)->snap_to_grid(128));
  }

  return best_snap.offset;
}

glm::vec2
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
    SnapData snap = wst.get_document().get_sector_model().snap_object(*i, ignore_objects);
    best_snap.merge(snap);
  }

  return best_snap.offset;
}

void
SelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  glm::vec2 pos = wst.get_state().screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y)));

  if (mode == CONTROL_DRAG_MODE)
  {
    ctrl_point->on_move_update(event, pos - click_pos);
    wst.queue_draw();
  }
  else if (mode == OBJECT_DRAG_MODE)
  {
    glm::vec2 offset = pos - click_pos;

    if ((event->time - start_time) > MOVE_TIMEOUT ||
        glm::length(offset) > MOVE_THRESHOLD)
    {
      if (!selection->is_moving())
        selection->on_move_start();

      selection->on_move_update(offset);

      if (event->state & GDK_CONTROL_MASK)
      {
        selection->on_move_update(offset + process_snap(wst));
      }
      else if (event->state & GDK_SHIFT_MASK)
      {
        selection->on_move_update(offset + process_grid_snap(wst));
      }

      wst.queue_draw();
    }
  }
  else if (mode == SELECT_MODE)
  {
    rect = Rectf(click_pos, pos);

    std::ostringstream str;
    str << "  (" << static_cast<int>(rect.left()) << ", " << static_cast<int>(rect.top()) << ")  "
        << abs(static_cast<int>(rect.width())) << " x " << abs(static_cast<int>(rect.height())) << "  ";
    EditorWindow::current()->print_coordinates(str.str());

    wst.queue_draw();
  }
}

void
SelectTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  glm::vec2 pos = wst.get_state().screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y)));

  // Select objects
  if (mode == CONTROL_DRAG_MODE)
  {
    ctrl_point->on_move_end(event, pos - click_pos);
    wst.get_document().create_control_points();
    wst.queue_draw();
  }
  else if (mode == OBJECT_DRAG_MODE)
  {
    glm::vec2 offset = pos - click_pos;

    if (event->time - start_time > MOVE_TIMEOUT ||
        glm::length(offset) > MOVE_THRESHOLD)
    {
      if (selection->is_moving())
      {
        selection->on_move_update(offset);

        if (event->state & GDK_CONTROL_MASK)
        {
          selection->on_move_end(wst, offset + process_snap(wst));
        }
        else if (event->state & GDK_SHIFT_MASK)
        {
          selection->on_move_end(wst, offset + process_grid_snap(wst));
        }
        else
        {
          selection->on_move_end(wst, offset);
        }
      }
      wst.queue_draw();
    }
  }
  else if (mode == SELECT_MODE)
  {
    mode = NO_MODE;
    rect = geom::normalize(rect);

    if (event->state & GDK_SHIFT_MASK)
    {
      SelectionHandle new_selection = wst.get_document().get_sector_model().get_selection(rect, wst.get_select_mask());
      wst.get_document().get_selection()->add(new_selection->begin(), new_selection->end());
    }
    else
    {
      wst.get_document().set_selection(wst.get_document().get_sector_model().get_selection(rect, wst.get_select_mask()));
    }
    wst.queue_draw();
  }

  EditorWindow::current()->print_coordinates(std::string());
  mode = NO_MODE;
}

void
SelectTool::mouse_right_down(GdkEventButton* event, WindstilleWidget& /*wst*/)
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
