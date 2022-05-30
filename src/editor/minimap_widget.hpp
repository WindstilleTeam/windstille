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

#include <gtkmm/widget.h>

class MinimapWidget : public Gtk::Widget
{
private:
public:
  MinimapWidget();

  bool mouse_move(GdkEventMotion* event);
  bool mouse_down (GdkEventButton* event);
  bool mouse_up(GdkEventButton* event);
  bool scroll(GdkEventScroll* event);

private:
  MinimapWidget(MinimapWidget const&);
  MinimapWidget& operator=(MinimapWidget const&);
};

#endif

/* EOF */
