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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_WIDGET_HPP

#include <gtkmm/drawingarea.h>
#include <boost/shared_ptr.hpp>

#include "math/vector2f.hpp"

class Timeline;

class TimelineWidget : public Gtk::DrawingArea
{
private:
  boost::shared_ptr<Timeline> m_timeline;

  Vector2f down_pos;
  Vector2f move_pos;

public:
  TimelineWidget();
  ~TimelineWidget();

  void set_timeline(boost::shared_ptr<Timeline> timeline);

  bool mouse_down(GdkEventButton* ev);
  bool mouse_up(GdkEventButton* ev);
  bool mouse_move(GdkEventMotion* ev);

protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);

private:
  TimelineWidget(const TimelineWidget&);
  TimelineWidget& operator=(const TimelineWidget&);
};

#endif

/* EOF */
