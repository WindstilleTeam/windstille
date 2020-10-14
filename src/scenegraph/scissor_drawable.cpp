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

#include "scenegraph/scissor_drawable.hpp"

#include <iostream>

#include "display/display.hpp"

ScissorDrawable::ScissorDrawable(const geom::irect& cliprect) :
  m_cliprect(cliprect), // FIXME: should we keep cliprect in world space instead of screen space?
  m_drawable_group()
{
}

void
ScissorDrawable::render(unsigned int mask)
{
  std::cout << "Render" << std::endl;
  Display::push_cliprect(m_cliprect);
  m_drawable_group.render(mask);
  Display::pop_cliprect();
}

/* EOF */
