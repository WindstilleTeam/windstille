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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_WIDGET_HPP

#include <gtkmm/drawingarea.h>
#include <memory>
#include <set>

#include <glm/glm.hpp>

#include <geom/geom.hpp>

#include "editor/timeline_handles.hpp"

class Timeline;

class TimelineWidget : public Gtk::DrawingArea
{
private:
  std::shared_ptr<Timeline>    m_timeline;
  std::set<TimelineObjectHandle> m_selection;

  enum {
    kNoMode,
    kSelectMode,
    kDragMode,
    kCursorSetMode
  } m_mode;

  glm::vec2 down_pos;
  glm::vec2 move_pos;

  float m_cursor_pos;
  int m_column_width;
  int m_column_height;

public:
  TimelineWidget();
  ~TimelineWidget() override;

  void set_timeline(TimelineHandle timeline);

  bool mouse_down(GdkEventButton* ev);
  bool mouse_up(GdkEventButton* ev);
  bool mouse_move(GdkEventMotion* ev);

  void  set_cursor_pos(float p);
  float get_cursor_pos() const;

  void delete_selection();

  void zoom_in();
  void zoom_out();

private:
  void draw_select_rectangle(Cairo::RefPtr<Cairo::Context> cr);
  void draw_grid(Cairo::RefPtr<Cairo::Context> cr);
  void draw_timeline(Cairo::RefPtr<Cairo::Context> cr);
  void draw_timeline_layer(Cairo::RefPtr<Cairo::Context> cr,
                           TimelineLayerHandle layer);
  void add_to_selection(const geom::frect& selection);

private:
  TimelineWidget(const TimelineWidget&);
  TimelineWidget& operator=(const TimelineWidget&);
};

#endif

/* EOF */
