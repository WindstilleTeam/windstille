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

#include <GL/glew.h>

#include "display/render_buffer.hpp"

class RenderBufferImpl
{
public:
  GLuint handle;

  RenderBufferImpl(GLenum format, int width, int height)
    : handle(0)
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
