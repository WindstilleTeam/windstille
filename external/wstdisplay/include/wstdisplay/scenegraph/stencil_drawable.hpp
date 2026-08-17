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

#ifndef HEADER_WINDSTILLE_STENCIL_DRAWABLE_HPP
#define HEADER_WINDSTILLE_STENCIL_DRAWABLE_HPP

#include <wstdisplay/scenegraph/drawable.hpp>
#include <wstdisplay/scenegraph/drawable_group.hpp>

namespace wstdisplay {

class StencilDrawable : public Drawable
{
private:
  DrawableGroup m_stencil_group;
  DrawableGroup m_drawable_group;

public:
  StencilDrawable();

  void render(GraphicsContext& gc, unsigned int mask) override;

  DrawableGroup& get_stencil_group()  { return m_stencil_group; }
  DrawableGroup& get_drawable_group() { return m_drawable_group; }

private:
  StencilDrawable(const StencilDrawable&);
  StencilDrawable& operator=(const StencilDrawable&);
};

} // namespace wstdisplay

#endif

/* EOF */
