/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_GLUTIL_SURFACE_DRAWING_PARAMETERS_HPP
#define HEADER_GLUTIL_SURFACE_DRAWING_PARAMETERS_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include "math/vector.hpp"
#include "color.hpp"

/** Helper class which holds all the parameters that you can tweak
    when you want to draw a Surface */
class SurfaceDrawingParameters
{
public:
  /** This includes flip, rotation, scaling and position */
  GLenum blendfunc_src;
  GLenum blendfunc_dst;

  Vector pos;

  /** Includes color and alpha settings */
  Color   color;

  float rotation;

  float scale;

  SurfaceDrawingParameters()
  {
    blendfunc_src = GL_SRC_ALPHA;
    blendfunc_dst = GL_ONE_MINUS_SRC_ALPHA;

    pos = Vector(0, 0);
    
    color = Color(1.0f, 1.0f, 1.0f, 1.0f);
  
    rotation = 0.0f;
    scale    = 1.0f;
  }

  SurfaceDrawingParameters& set_scale(float s) 
  { 
    scale    = s; 
    return *this; 
  }

  SurfaceDrawingParameters& set_color(const Color& c)
  { 
    color = c;
    return *this; 
  }

  SurfaceDrawingParameters& set_rotation(float r) 
  { 
    rotation = r; 
    return *this; 
  }

  SurfaceDrawingParameters& set_pos(const Vector& p) 
  { 
    pos = p;
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
