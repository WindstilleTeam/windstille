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

#include "editor/minimap_widget.hpp"

MinimapWidget::MinimapWidget(const Glib::RefPtr<const Gdk::GL::Config>& glconfig)
{
  set_gl_capability(glconfig);
  set_size_request(0, 64);
}

bool
MinimapWidget::mouse_move(GdkEventMotion* /*event*/)
{
  return false;
}

bool
MinimapWidget::mouse_down (GdkEventButton* /*event*/)
{
  return false;
}

bool
MinimapWidget::mouse_up(GdkEventButton* /*event*/)
{
  return false;
}

bool
MinimapWidget::scroll(GdkEventScroll* /*event*/)
{
  return false;
}

bool
MinimapWidget::on_expose_event(GdkEventExpose* /*event*/)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (!glwindow->gl_begin(get_gl_context()))
  {
    return false;
  }
  else
  {
    //glClear(GL_COLOR_BUFFER_BIT);
    //glwindow->swap_buffers();
    glwindow->gl_end();

    return true;
  }
}

/* EOF */
