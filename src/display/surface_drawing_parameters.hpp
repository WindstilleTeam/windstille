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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_DRAWING_PARAMETERS_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_DRAWING_PARAMETERS_HPP

#include <GL/glew.h>
#include <GL/gl.h>

#include "math/vector2f.hpp"
#include "display/color.hpp"

/** Helper class which holds all the parameters that you can tweak
    when you want to draw a Surface */
class SurfaceDrawingParameters
{
public:
  /** This includes flip, rotation, scaling and position */
  GLenum blendfunc_src;
  GLenum blendfunc_dst;

  Vector2f pos;
  float    z_pos;

  /** Includes color and alpha settings */
  Color   color;

  float angle;

  Vector2f scale;

  bool hflip;
  bool vflip;

  SurfaceDrawingParameters()
    : blendfunc_src(GL_SRC_ALPHA),
      blendfunc_dst(GL_ONE_MINUS_SRC_ALPHA),
      pos(0, 0),
      z_pos(0.0f),
      color(1.0f, 1.0f, 1.0f, 1.0f),
      angle(0.0f),
      scale(1.0f, 1.0f),
      hflip(false),
      vflip(false)
  {}

  SurfaceDrawingParameters& set_scale(float s) 
  { 
    scale    = Vector2f(s, s); 
    return *this; 
  }

  SurfaceDrawingParameters& set_color(const Color& c)
  { 
    color = c;
    return *this; 
  }

  SurfaceDrawingParameters& set_angle(float r) 
  { 
    angle = r;
    return *this; 
  }

  SurfaceDrawingParameters& set_scale(const Vector2f& s) 
  { 
    scale = s;
    return *this; 
  }

  SurfaceDrawingParameters& set_pos(const Vector2f& p) 
  { 
    pos = p;
    return *this;
  }

  SurfaceDrawingParameters& set_z_pos(float p)
  { 
    z_pos = p;
    return *this;
  }

  SurfaceDrawingParameters& set_hflip(bool v)
  {
    hflip = v;
    return *this;
  }

  SurfaceDrawingParameters& set_vflip(bool v)
  {
    vflip = v;
    return *this;
  }
  
  SurfaceDrawingParameters& set_blend_func(GLenum src, GLenum dst) 
  { 
    blendfunc_src = src;
    blendfunc_dst = dst;
    return *this; 
  }  
};

#endif

/* EOF */
