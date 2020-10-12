/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "gui/button.hpp"

#include <wstinput/controller.hpp>

#include "app/controller_def.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "gui/tab_component.hpp"
#include "util/log.hpp"

namespace gui {

Button::Button(Component* parent_)
  : Component(parent_),
    label()
{
  log_error("implement me");
}

Button::Button(const std::string& label_, Component* parent_)
  : Component(Rectf(), parent_),
    label(label_)
{
}

Button::~Button()
{
}

void
Button::draw()
{
  Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  Fonts::current()->vera20->draw_center(Vector2f(rect.left() + rect.width()/2, rect.top() + rect.height()/2),
                                        label,
                                        is_active()
                                        ? Color(1.0f, 1.0f, 1.0f, 1.0f)
                                        : Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
Button::update(float , const Controller& controller)
{
  set_active(false);

  for(auto i = controller.get_events().begin(); i != controller.get_events().end(); ++i)
  {
    if (i->type == wstinput::BUTTON_EVENT && i->button.down)
    {
      if (i->button.name == OK_BUTTON)
      {
      }
      else if (i->button.name == CANCEL_BUTTON || i->button.name == ESCAPE_BUTTON)
      {
        set_active(false);
      }
    }
  }
}

} // namespace gui

/* EOF */
