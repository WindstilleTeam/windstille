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

#include "editor/timeline_widget.hpp"

#include <iostream>
#include <sstream>

#include "math/rect.hpp"
#include "editor/editor_window.hpp"
#include "editor/windstille_widget.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_layer.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_keyframe_object.hpp"

TimelineWidget::TimelineWidget() :
  m_timeline(),
  m_selection(),
  m_mode(kNoMode),
  down_pos(),
  move_pos(),
  m_cursor_pos(0.0f),
  m_column_width(8),
  m_column_height(32)
{
  signal_button_release_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_up));
  signal_button_press_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_down));
  signal_motion_notify_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_move));

  add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::KEY_PRESS_MASK      | Gdk::KEY_RELEASE_MASK |
             Gdk::ENTER_NOTIFY_MASK   | Gdk::LEAVE_NOTIFY_MASK);
}

TimelineWidget::~TimelineWidget()
{
}

bool
TimelineWidget::mouse_down(GdkEventButton* ev)
{
  if (ev->button == 1)
  {
    down_pos.x = static_cast<float>(ev->x);
    down_pos.y = static_cast<float>(ev->y);
    move_pos.x = static_cast<float>(ev->x);
    move_pos.y = static_cast<float>(ev->y);

    TimelineLayerHandle layer = m_timeline->get_layer(static_cast<int>(ev->y / m_column_height));
    if (layer)
    {
      TimelineObjectHandle obj = layer->get_object(static_cast<float>(ev->x / m_column_width));
      if (obj)
      {
        m_mode = kDragMode;

        if (m_selection.find(obj) == m_selection.end())
        {
          if (!(ev->state & GDK_SHIFT_MASK))
            m_selection.clear();

          m_selection.insert(obj);
        }
        else
        {
          if (ev->state & GDK_SHIFT_MASK)
          {
            m_selection.erase(m_selection.find(obj));
          }
        }
      }
      else
        m_mode = kSelectMode;
    }
    else
    {
      m_mode = kSelectMode;
    }
  }
  else if (ev->button == 2)
  { // scroll

  }
  else if (ev->button == 3)
  { // set cursor
    set_cursor_pos(floorf(static_cast<float>(ev->x / m_column_width)));
    m_mode = kCursorSetMode;
    queue_draw();
  }
  return false;
}

bool
TimelineWidget::mouse_up(GdkEventButton* ev)
{
  if (ev->button == 1)
  {
    if (m_mode == kSelectMode)
    {
      m_mode = kNoMode;

      Rectf selection(down_pos, Vector2f(static_cast<float>(ev->x), static_cast<float>(ev->y)));
      selection.normalize();

      if (!(ev->state & GDK_SHIFT_MASK))
        m_selection.clear();

      add_to_selection(selection);
    }
    else if (m_mode == kDragMode)
    {
      m_mode = kNoMode;
      for (std::set<TimelineObjectHandle>::iterator i = m_selection.begin();
           i != m_selection.end(); ++i)
      {
        (*i)->set_pos((*i)->get_pos() + (move_pos.x - down_pos.x) / static_cast<float>(m_column_width));
      }
    }
    queue_draw();
  }
  else if (ev->button == 3)
  {
    m_mode = kNoMode;
  }

  return false;
}

bool
TimelineWidget::mouse_move(GdkEventMotion* ev)
{
  if (m_mode == kSelectMode)
  {
    move_pos.x = static_cast<float>(ev->x);
    move_pos.y = static_cast<float>(ev->y);
    queue_draw();
  }
  else if (m_mode == kDragMode)
  {
    move_pos.x = static_cast<float>(ev->x);
    move_pos.y = static_cast<float>(ev->y);

    queue_draw();
  }
  else if (m_mode == kCursorSetMode)
  {
    set_cursor_pos(floorf(static_cast<float>(ev->x / m_column_width)));
    queue_draw();
  }

  return false;
}

void
TimelineWidget::add_to_selection(const Rectf& selection)
{
  Timeline::iterator start = m_timeline->begin() +
    std::max(0, std::min(m_timeline->size(),
                         static_cast<int>((selection.top + static_cast<float>(m_column_height)/2) / static_cast<float>(m_column_height))));
  Timeline::iterator end   = m_timeline->begin() +
    std::max(0, std::min(m_timeline->size(),
                         static_cast<int>((selection.bottom + static_cast<float>(m_column_height)/2) / static_cast<float>(m_column_height))));

  for(Timeline::iterator i = start; i != end; ++i)
  {
    const TimelineLayer::Objects& objects
      = (*i)->get_objects(selection.left / static_cast<float>(m_column_width), selection.right / static_cast<float>(m_column_width));
    m_selection.insert(objects.begin(), objects.end());
  }
}

bool
TimelineWidget::on_expose_event(GdkEventExpose* ev)
{
  if (Glib::RefPtr<Gdk::Window> window = get_window())
  {
    //Gtk::Allocation allocation = get_allocation();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    cr->rectangle(ev->area.x,     ev->area.y,
                  ev->area.width, ev->area.height);
    cr->clip();

    if (1) // pixel perfect drawing
      cr->translate(0.5, 0.5);

    draw_timeline(cr);

    cr->set_line_width(0.5);
    cr->set_source_rgb(0,0,0);

    cr->set_line_width(1.0);

    draw_select_rectangle(cr);
  }

  return true;
}

void
TimelineWidget::draw_select_rectangle(Cairo::RefPtr<Cairo::Context> cr)
{
  // Select rectangle
  if (m_mode == kSelectMode)
  {
    //Rectf rect(down_pos, move_pos - down_pos);
    //rect.normalize();

    cr->set_source_rgba(0,0,1,0.25);
    cr->rectangle(down_pos.x, down_pos.y,
                  move_pos.x - down_pos.x,
                  move_pos.y - down_pos.y);
    cr->fill();

    cr->set_source_rgb(0,0,1);
    cr->rectangle(down_pos.x, down_pos.y,
                  move_pos.x - down_pos.x,
                  move_pos.y - down_pos.y);
    cr->stroke();
  }
}

void
TimelineWidget::draw_grid(Cairo::RefPtr<Cairo::Context> cr)
{
  Gtk::Allocation allocation = get_allocation();

  cr->save();

  int height = m_timeline->size() * m_column_height;

  // draw vertical lines
  cr->set_source_rgb(0.875, 0.875, 0.875);
  for(int x = 0; x < allocation.get_width(); x += m_column_width)
  {
    if (x % (m_column_width * 10) == 0)
    {
      cr->rectangle(x, 0, static_cast<float>(m_column_width), height);
    }
  }
  cr->fill();

  cr->set_line_width(1.0);

  // draw horizontal lines
  cr->set_source_rgb(0.5, 0.5, 0.5);
  for(int y = 0; y <= height; y += m_column_height)
  {
    cr->move_to(0, y);
    cr->line_to(allocation.get_width(), y);
  }
  cr->stroke();

  // draw vertical lines
  cr->set_source_rgb(0.75, 0.75, 0.75);
  for(int x = 0; x < allocation.get_width(); x += m_column_width)
  {
    cr->move_to(x, 0);
    cr->line_to(x, height);
  }
  cr->stroke();

  cr->restore();
}

void
TimelineWidget::draw_timeline(Cairo::RefPtr<Cairo::Context> cr)
{
  Gtk::Allocation allocation = get_allocation();

  cr->save();

  cr->set_source_rgb(1,1,1);
  cr->rectangle(0, 0, allocation.get_width(), static_cast<float>(m_column_height) * static_cast<float>(m_timeline->size()));
  cr->fill();

  draw_grid(cr);

  cr->save();
  for(Timeline::iterator i = m_timeline->begin(); i != m_timeline->end(); ++i)
  {
    draw_timeline_layer(cr, *i);
    cr->translate(0, static_cast<float>(m_column_height));
  }
  cr->restore();

  cr->rectangle(m_cursor_pos * static_cast<float>(m_column_width), 0,
                static_cast<float>(m_column_width), static_cast<float>(m_timeline->size()) * static_cast<float>(m_column_height));
  cr->set_source_rgba(1,1,0,0.5);
  cr->fill_preserve();
  cr->set_line_width(1.0f);
  cr->set_source_rgb(0,0,0);
  cr->stroke();

  cr->restore();
}

void
TimelineWidget::draw_timeline_layer(Cairo::RefPtr<Cairo::Context> cr,
                                    TimelineLayerHandle layer)
{
  cr->save();

  cr->set_line_width(1.0);

  for(TimelineLayer::iterator i = layer->begin(); i != layer->end(); ++i)
  {
    bool in_selection = m_selection.find(*i) != m_selection.end();

    if (std::shared_ptr<TimelineKeyframeObject> keyframe =
        std::dynamic_pointer_cast<TimelineKeyframeObject>(*i))
    {
      cr->save();

      if (in_selection)
      {
        cr->set_source_rgb(0.75, 1.0, 0.0);
        if (m_mode == kDragMode)
          cr->translate(move_pos.x - down_pos.x, 0.0);
      }
      else
      {
        cr->set_source_rgb(0.5, 0.75, 0.0);
      }

      cr->rectangle(keyframe->get_pos() * static_cast<float>(m_column_width),
                    4.0f, static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8.0f);
      cr->fill();

      if (in_selection)
        cr->set_source_rgb(0.25, 0.25, 0.25);
      else
        cr->set_source_rgb(0, 0, 0);

      cr->rectangle(keyframe->get_pos() * static_cast<float>(m_column_width), 4.0f,
                    static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8.0f);
      cr->stroke();

      cr->restore();
    }
    else if (std::shared_ptr<TimelineAnimObject> anim =
             std::dynamic_pointer_cast<TimelineAnimObject>(*i))
    {
      cr->save();

      Cairo::TextExtents extents;

      cr->select_font_face ("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size(12);

      cr->get_text_extents(anim->get_name(), extents);

      if (0)
        std::cout << extents.x_bearing << " "
                  << extents.y_bearing << " "
                  << extents.width << " "
                  << extents.height << " "
                  << extents.x_advance << " "
                  << extents.y_advance << " "
                  << std::endl;

      if (in_selection)
      {
        cr->set_source_rgb(0.0, 0.75, 1.0);
        if (m_mode == kDragMode)
          cr->translate(move_pos.x - down_pos.x, 0.0);
      }
      else
      {
        cr->set_source_rgb(0.0, 0.5, 0.75);
      }

      cr->rectangle(anim->get_pos()   * static_cast<float>(m_column_width),  4,
                    anim->get_width() * static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8);
      cr->fill();

      if (in_selection)
      {
        cr->set_source_rgb(0.25, 0.25, 0.25);
      }
      else
      {
        cr->set_source_rgb(0, 0, 0);
      }

      cr->rectangle(anim->get_pos()   * static_cast<float>(m_column_width), 4,
                    anim->get_width() * static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8);
      cr->stroke();

      cr->set_source_rgb(1,1,1);
      cr->move_to((anim->get_pos() + anim->get_width()/2) * static_cast<float>(m_column_width) - static_cast<float>(extents.width) / 2,
                  static_cast<float>(m_column_height) / 2 + 4);
      cr->show_text(anim->get_name());

      cr->restore();
    }
    else if (std::shared_ptr<TimelineSoundObject> sound =
             std::dynamic_pointer_cast<TimelineSoundObject>(*i))
    {
      cr->save();

      Cairo::TextExtents extents;

      cr->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size(12);

      cr->get_text_extents(sound->get_name(), extents);

      if (0)
        std::cout << extents.x_bearing << " "
                  << extents.y_bearing << " "
                  << extents.width << " "
                  << extents.height << " "
                  << extents.x_advance << " "
                  << extents.y_advance << " "
                  << std::endl;

      if (in_selection)
      {
        if (m_mode == kDragMode)
          cr->translate(move_pos.x - down_pos.x, 0.0);

        cr->set_source_rgb(1.0, 0.0, 0.75);
      }
      else
      {
        cr->set_source_rgb(0.75, 0.0, 0.5);
      }

      cr->rectangle(sound->get_pos()   * static_cast<float>(m_column_width),  4,
                    sound->get_width() * static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(sound->get_pos()   * static_cast<float>(m_column_width), 4,
                    sound->get_width() * static_cast<float>(m_column_width), static_cast<float>(m_column_height) - 8);
      cr->stroke();

      cr->set_source_rgb(1,1,0);
      cr->move_to((sound->get_pos() + sound->get_width()/2) * static_cast<float>(m_column_width) - static_cast<float>(extents.width) / 2.0f,
                  static_cast<float>(m_column_height) / 2 + 4);
      cr->show_text(sound->get_name());

      cr->restore();
    }
  }

  cr->restore();
}

void
TimelineWidget::delete_selection()
{

}

void
TimelineWidget::zoom_in()
{
  m_column_width += 1;
  queue_draw();
}

void
TimelineWidget::zoom_out()
{
  if (m_column_width > 2)
  {
    m_column_width -= 1;
    queue_draw();
  }
}

void
TimelineWidget::set_timeline(std::shared_ptr<Timeline> timeline)
{
  m_timeline = timeline;
}

void
TimelineWidget::set_cursor_pos(float p)
{
  m_cursor_pos = p;
  m_timeline->apply(p);
  if (WindstilleWidget* wst = EditorWindow::current()->get_windstille_widget())
    wst->queue_draw();
}

float
TimelineWidget::get_cursor_pos() const
{
  return m_cursor_pos;
}

/* EOF */
