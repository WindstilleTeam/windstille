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

#ifndef HEADER_WINDSTILLE_DISPLAY_DRAWING_PARAMETERS_HPP
#define HEADER_WINDSTILLE_DISPLAY_DRAWING_PARAMETERS_HPP

#include <GL/glew.h>

#include <surf/color.hpp>

namespace wstdisplay {

/** Helper class which holds all the parameters that you can tweak
    when you want to draw a Quad */
class DrawingParameters
{
public:
  /** This includes flip, rotation, scaling and position */
  GLenum blendfunc_src;
  GLenum blendfunc_dst;

  /** Includes color and alpha settings */
  surf::Color   color;

  DrawingParameters()
    : blendfunc_src(GL_SRC_ALPHA),
      blendfunc_dst(GL_ONE_MINUS_SRC_ALPHA),
      color(1.0f, 1.0f, 1.0f, 1.0f)
  {}

  DrawingParameters& set_color(const surf::Color& c)
  {
    color = c;
    return *this;
  }

  DrawingParameters& set_blend_func(GLenum src, GLenum dst)
  {
    blendfunc_src = src;
    blendfunc_dst = dst;
    return *this;
  }
};

} // namespace wstdisplay

#endif

/* EOF */
