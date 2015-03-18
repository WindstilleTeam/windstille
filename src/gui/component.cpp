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

#include "gui/component.hpp"

namespace gui {

Component::Component(Component* parent_)
  : rect(),
    parent(parent_),
    active(false)
{
}

Component::Component(const Rectf& rect_, Component* parent_)
  : rect(rect_),
    parent(parent_),
    active(false)
{
}

Component::~Component()
{
}

bool
Component::is_active() const
{
  return active;
}

void
Component::set_active(bool a)
{
  if (!active && a)
    on_activation();
  active = a;
}

Rectf
Component::get_screen_rect() const
{
  return rect;
}

void
Component::set_screen_rect(const Rectf& r)
{
  rect = r;
}

float
Component::get_prefered_width() const
{
  assert(!"Implement me");
  return 100.0f;
}

float
Component::get_prefered_height() const
{
  assert(!"Implement me");
  return 100.0f;
}

} // namespace gui

/* EOF */
