/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_DRAWING_REQUEST_HXX
#define HEADER_DRAWING_REQUEST_HXX

#include "math/vector.hpp"
#include "math/rect.hpp"
#include "math/matrix.hpp"
#include "texture.hpp"

/** 
 */
class DrawingRequest
{
protected:
  Vector  pos;
  float   z_pos;
  Matrix  modelview;

public:
  DrawingRequest(const Vector& pos_, float z_pos = 0,  const Matrix& modelview_ = Matrix::identity())
    : pos(pos_), z_pos(z_pos), modelview(modelview_)
  {}
  virtual ~DrawingRequest() {}
  
  /**
   * The draw() method does the important work in DrawingRequest,
   * ie. it is the place where you can access the screen with raw
   * OpenGL methods. The \a tmp_texture provides a texture of the
   * current framebuffer, you have to copy the \a screen_texture to it
   * to contain usefull content
   */
  virtual void draw(const Texture& tmp_texture) = 0;

  /**
   * This method is called before draw() to allow the DrawingRequest
   * to copy content from \a screen_texture, which is the current
   * framebuffer to a temporary buffer which can then be used in
   * draw() for deformation effects
   */
  virtual void prepare(const Texture& screen_texture) {
      (void) screen_texture;
  }

  /**
   * Override this and let it return true if you need to prepare()
   * function call
   */
  virtual bool needs_prepare() { return false; }
  
  /** Returns the position at which the request should be drawn */
  float get_z_pos() const { return z_pos; }

  Matrix get_modelview() const
  { return modelview; }

private:
  DrawingRequest (const DrawingRequest&);
  DrawingRequest& operator= (const DrawingRequest&);
};

#endif

/* EOF */
