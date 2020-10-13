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

#include "app/app.hpp"
#include "font/fonts.hpp"
#include "gui/label.hpp"

namespace gui {

Label::Label(const std::string& label_, Component* parent_)
  : Component(Rectf(), parent_),
    label(label_)
{
}

Label::~Label()
{
}

void
Label::draw()
{
  //Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  //Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  g_app.fonts().vera12->draw(glm::vec2(rect.left() + 5/*+ rect.width()/2*/, rect.top() + rect.height()/2 + 3),
                                 label,
                                 Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void
Label::update(float /*delta*/, const Controller& /*controller*/)
{
  set_active(false);
}

} // namespace gui

/* EOF */
