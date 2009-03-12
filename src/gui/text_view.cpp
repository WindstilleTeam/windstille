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

#include <iostream>
#include "input/controller.hpp"
#include "text_view.hpp"

namespace gui {

TextView::TextView(const Rectf& rect, Component* component)
  : Component(rect, component),
    text_area(rect, false)
{
}

TextView::~TextView()
{
}
  
void
TextView::draw()
{
  text_area.draw();
}

void
TextView::update(float delta, const Controller& controller)
{
  text_area.update(delta);

  text_area.set_scroll_offset(text_area.get_scroll_offset() + 500.0f * controller.get_axis_state(Y2_AXIS) * delta);

  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT && i->button.down)
        {
          if (i->button.name == OK_BUTTON || i->button.name == ENTER_BUTTON)
            {
              set_active(false);
            }
          else if (i->button.name == CANCEL_BUTTON || i->button.name == ESCAPE_BUTTON)
            {            
              set_active(false);
            }
        }
    }  
}

void
TextView::set_screen_rect(const Rectf& rect)
{
  Component::set_screen_rect(rect);
  text_area.set_rect(rect.grow(-16.0f, -4.0f));
}

void
TextView::set_text(const std::string& text)
{
  text_area.set_text(text);
}

void
TextView::set_font(TTFFont* font)
{
  text_area.set_font(font);
}

} // namespace gui

/* EOF */
