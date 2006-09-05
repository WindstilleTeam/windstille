/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "input/input_manager.hpp"
#include "display/display.hpp"
#include "text_area.hpp"
#include "font/fonts.hpp"
#include "controller_help_window.hpp"

ControllerHelpWindow* ControllerHelpWindow::current_ = 0;

class ControllerHelpWindowImpl
{
public:
  TextArea* text_area;
  bool active;
};

ControllerHelpWindow::ControllerHelpWindow()
  : impl(new ControllerHelpWindowImpl)
{
  current_ = this;
  int width  = 200;
  int height = 120;
  
  impl->active = false;
  impl->text_area = new TextArea(Rect(Point(Display::get_width() - width - 16,
                                            Display::get_height() - height - 16),
                                      Size(width, height)), false);
  impl->text_area->set_font(Fonts::ttffont);
  impl->text_area->set_text("Control Help\n"
                            "------------\n"
                            "walk: left/right\n"
                            "duck: down\n\n"
                            "primary/run:    s\n"
                            "secondary/jump: d\n"
                            "tertiary:       a\n"
                            "pda:            w\n"
                            );
}

void
ControllerHelpWindow::draw()
{
  if (!impl->active)
    return;
    
  const Rectf& rect = impl->text_area->get_rect().grow(8.0f);

  Display::fill_rounded_rect(rect, 16.0f, Color(0.3f, 0.3f, 0.5f, 0.5f));
  Display::draw_rounded_rect(rect, 16.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
  impl->text_area->draw();

  Controller controller = InputManager::get_controller();
  
  Rectf crect(Vector(rect.right - 100, rect.top - 100 - 8.0f),
              Size(100, 100));

  Display::fill_rounded_rect(crect, 10.0f, Color(1.0f, 1.0f, 1.0f, 0.2f));
  Display::draw_rounded_rect(crect, 10.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  Vector pos(crect.left + crect.get_width()/2  + controller.get_axis_state(X_AXIS) * (crect.get_width()-16.0f)/2,
             crect.top  + crect.get_height()/2 + controller.get_axis_state(Y_AXIS)   * (crect.get_width()-16.0f)/2);

  Display::fill_circle(pos, 10.0f, Color(0.8f, 0, 0));
  Display::draw_circle(pos, 10.0f, Color(1.0f, 0, 0));

  for(int i = 0; i < 6; ++i)
    {
      if (controller.get_button_state(i))
        {
          Display::fill_circle(Vector(crect.left - 16.0f, crect.top + (crect.get_height()-20)/3 * i + 10.0f),
                               10.0f, Color(0.8f, 0, 0));
          Display::draw_circle(Vector(crect.left - 16.0f, crect.top + (crect.get_height()-20)/3 * i + 10.0f),
                               10.0f, Color(1.0f, 0.0f, 0.0f));
        }
      else
        {
          Display::fill_circle(Vector(crect.left - 16.0f, crect.top + (crect.get_height()-20)/3 * i + 10.0f),
                               10.0f, Color(1.0f, 1.0f, 1.0f, 0.2f));
          Display::draw_circle(Vector(crect.left - 16.0f, crect.top + (crect.get_height()-20)/3 * i + 10.0f),
                               10.0f,  Color(1.0f, 1.0f, 1.0f, 0.5f));
        }
    }
}

void
ControllerHelpWindow::update(float delta, const Controller& )
{
  impl->text_area->update(delta);
}

void
ControllerHelpWindow::set_active(bool active)
{
  impl->active = active;
}

/* EOF */
