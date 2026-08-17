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

#include "button.hpp"

#include <logmich/log.hpp>
#include <wstinput/controller.hpp>
#include <wstdisplay/graphics_context.hpp>

#include "controller_def.hpp"
#include "tab_component.hpp"

namespace wstgui {

Button::Button(const std::string& label_, Component* parent_) :
  Component(parent_),
  label(label_)
{
}

Button::~Button()
{
}

void
Button::draw(wstdisplay::GraphicsContext& gc)
{
  gc.fill_rect(m_geometry, surf::Color(0.0f, 0.0f, 0.0f, 0.5f));
  gc.draw_rect(m_geometry, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  get_style().get_font()->draw_center(
    gc,
    glm::vec2(m_geometry.left() + m_geometry.width()/2.0f,
              m_geometry.top() + m_geometry.height()/2.0f + static_cast<float>(get_style().get_font()->get_height()) / 3.0f),
    label,
    is_active()
    ? surf::Color(1.0f, 1.0f, 1.0f, 1.0f)
    : surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
Button::update(float delta, const Controller& controller)
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

} // namespace wstgui

/* EOF */
