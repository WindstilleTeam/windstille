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

#ifndef HEADER_WINDSTILLE_SCISSOR_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCISSOR_DRAWABLE_HPP

#include <wstdisplay/scenegraph/drawable.hpp>
#include <wstdisplay/scenegraph/drawable_group.hpp>

namespace wstdisplay {

class ScissorDrawable : public Drawable
{
private:
  geom::irect m_cliprect;
  DrawableGroup m_drawable_group;

public:
  ScissorDrawable(const geom::irect& cliprect);

  void render(GraphicsContext& gc, unsigned int mask) override;

  DrawableGroup& get_drawable_group() { return m_drawable_group; }

private:
  ScissorDrawable(const ScissorDrawable&);
  ScissorDrawable& operator=(const ScissorDrawable&);
};

} // namespace wstdisplay

#endif

/* EOF */
