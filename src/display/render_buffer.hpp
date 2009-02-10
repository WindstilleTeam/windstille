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

#ifndef HEADER_RENDER_BUFFER_HPP
#define HEADER_RENDER_BUFFER_HPP

#include <GL/gl.h>
#include <boost/shared_ptr.hpp>

class RenderBufferImpl;

/** */
class RenderBuffer
{
public:
  /**
   * @param width  Width of the Renderbuffer
   * @param height Height of the Renderbuffer
   * @param format GL_DEPTH_COMPONENT24, GL_STENCIL_INDEX, ...
   */
  RenderBuffer(GLenum format, int width, int height);
  
  GLuint get_handle() const;

private:
  boost::shared_ptr<RenderBufferImpl> impl;
};

#endif

/* EOF */
