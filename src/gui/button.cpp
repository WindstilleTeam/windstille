/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "input/controller.hpp"
#include "button.hpp"

namespace gui {

Button::Button(Component* parent)
  : Component(parent)
{
  assert(0);
}

Button::Button(const std::string& label_, Component* parent)
  : Component(Rectf(), parent),
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
  Fonts::current()->vera20->draw_center(Vector2f(rect.left + rect.get_width()/2, rect.top + rect.get_height()/2),
                                        label,
                                        is_active()
                                        ? Color(1.0f, 1.0f, 1.0f, 1.0f) 
                                        : Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
Button::update(float , const Controller& controller)
{
  set_active(false);

  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT && i->button.down)
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
