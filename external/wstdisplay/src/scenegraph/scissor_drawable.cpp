// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "scenegraph/scissor_drawable.hpp"

#include <iostream>

#include "graphics_context.hpp"

namespace wstdisplay {

ScissorDrawable::ScissorDrawable(const geom::irect& cliprect) :
  m_cliprect(cliprect), // FIXME: should we keep cliprect in world space instead of screen space?
  m_drawable_group()
{
}

void
ScissorDrawable::render(GraphicsContext& gc, unsigned int mask)
{
  std::cout << "Render" << std::endl;
  gc.push_cliprect(m_cliprect);
  m_drawable_group.render(gc, mask);
  gc.pop_cliprect();
}

} // namespace wstdisplay

/* EOF */
