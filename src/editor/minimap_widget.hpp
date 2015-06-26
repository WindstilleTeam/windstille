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

#ifndef HEADER_WINDSTILLE_EDITOR_MINIMAP_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_MINIMAP_WIDGET_HPP

#include <gtkglmm.h>

/** Minimap Widget,
 */
class MinimapWidget
  : public Gtk::DrawingArea,
    public Gtk::GL::Widget<MinimapWidget>
{
private:
public:
  MinimapWidget(const Glib::RefPtr<const Gdk::GL::Config>& glconfig);

  bool mouse_move(GdkEventMotion* event);
  bool mouse_down (GdkEventButton* event);
  bool mouse_up(GdkEventButton* event);
  bool scroll(GdkEventScroll* event);

  bool on_expose_event(GdkEventExpose* event);

private:
  MinimapWidget(const MinimapWidget&);
  MinimapWidget& operator=(const MinimapWidget&);
};

#endif

/* EOF */
