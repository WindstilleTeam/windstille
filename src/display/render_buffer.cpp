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
#include <config.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "render_buffer.hpp"

class RenderBufferImpl
{
public:
  GLuint handle;

  RenderBufferImpl(GLenum format, int width, int height)
  {
    glGenRenderbuffersEXT(1, &handle);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, handle);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, format, width, height);
    
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
  }

  ~RenderBufferImpl()
  {
    glDeleteRenderbuffersEXT(1, &handle);
  }
};

RenderBuffer::RenderBuffer(GLenum format, int width, int height)
  : impl(new RenderBufferImpl(format, width, height))
{
}

GLuint
RenderBuffer::get_handle() const
{
  return impl->handle;
}

/* EOF */
