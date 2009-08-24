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
#include "util/util.hpp"
#include "render_buffer.hpp"
#include "framebuffer.hpp"

class FramebufferImpl
{
public:
  GLuint  handle;
  Texture texture;
  RenderBuffer render_buffer;
  
  FramebufferImpl(GLenum target, int width, int height)
    : handle(0),
      texture(target, width, height),
      render_buffer(GL_DEPTH_COMPONENT24, width, height)
  {
    glGenFramebuffersEXT(1, &handle);
    assert_gl("Framebuffer1");

    // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, handle);
    assert_gl("Framebuffer2");
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                              GL_COLOR_ATTACHMENT0_EXT, texture.get_target(), texture.get_handle(), 0);
    assert_gl("Framebuffer3");
    
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 
                                 GL_DEPTH_ATTACHMENT_EXT, // FIXME: must not hardcode this
                                 GL_RENDERBUFFER_EXT, render_buffer.get_handle());

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  }

  ~FramebufferImpl()
  {
    glDeleteFramebuffersEXT(1, &handle);
  }
};

Framebuffer::Framebuffer()
  : impl()
{
}

Framebuffer::Framebuffer(GLenum target, int width, int height)
  : impl(new FramebufferImpl(target, width, height))
{  
}

Framebuffer::~Framebuffer()
{
  
}

Texture
Framebuffer::get_texture()
{
  return impl->texture;
}

int
Framebuffer::get_width()  const
{
  return impl->texture.get_width();
}

int
Framebuffer::get_height() const
{
  return impl->texture.get_height();
}

GLuint
Framebuffer::get_handle() const
{
  return impl->handle;

}

Framebuffer::operator bool() const
{
  return impl.get();
}

bool
Framebuffer::operator==(const Framebuffer& other) const
{
  return impl.get() == other.impl.get();
}

bool
Framebuffer::operator!=(const Framebuffer& other) const
{
  return impl.get() != other.impl.get();
}

/* EOF */
