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

#include "math/size.hpp"
#include "util/util.hpp"
#include "display/render_buffer.hpp"
#include "display/framebuffer.hpp"
#include "display/assert_gl.hpp"

FramebufferPtr
Framebuffer::create_with_texture(GLenum target, int width, int height, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_with_texture_internal(target, width, height, multisample);
  return framebuffer;
}

FramebufferPtr
Framebuffer::create(int width, int height, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_internal(GL_RGB8, width, height, multisample);
  return framebuffer;
}

FramebufferPtr
Framebuffer::create_hdr(int width, int height, int multisample)
{
  FramebufferPtr framebuffer(new Framebuffer);
  framebuffer->create_internal(GL_RGBA16F, width, height, multisample);
  return framebuffer;
}

Framebuffer::Framebuffer() :
  m_handle(0),
  m_size(),
  m_texture(),
  m_color_buffer(),
  m_depth_stencil_buffer()
{
  glGenFramebuffersEXT(1, &m_handle);
  assert_gl("Framebuffer::Framebuffer()");   
}

Framebuffer::~Framebuffer()
{
  glDeleteFramebuffersEXT(1, &m_handle);  
}

Texture
Framebuffer::get_texture()
{
  assert(m_texture);
  return m_texture;
}

int
Framebuffer::get_width()  const
{
  return m_size.width;
}

int
Framebuffer::get_height() const
{
  return m_size.height;
}

GLuint
Framebuffer::get_handle() const
{
  return m_handle;

}

void
Framebuffer::create_with_texture_internal(GLenum target, int width, int height, int multisample)
{
  m_size = Size(width, height);
  m_texture = Texture(target, width, height);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8_EXT, width, height, multisample);
    
  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_handle);

  // bind texture and renderbuffers to the framebuffer
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_texture.get_target(), m_texture.get_handle(), 0);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_RENDERBUFFER_EXT, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depth_stencil_buffer->get_handle());

  assert_gl("Framebuffer::Framebuffer() - binding");

  check_completness();

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void
Framebuffer::create_internal(GLenum format, int width, int height, int multisample)
{ 
  m_size = Size(width, height);
  m_color_buffer = Renderbuffer::create(format, width, height, multisample);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8_EXT, width, height, multisample);

  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_handle);

  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,  GL_RENDERBUFFER_EXT, m_color_buffer->get_handle());
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_RENDERBUFFER_EXT, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depth_stencil_buffer->get_handle());

  assert_gl("Framebuffer::Framebuffer() - binding");

  check_completness();

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);    
}

void
Framebuffer::check_completness()
{
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status)
  {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      std::cout << "Framebuffer ok" << std::endl;
      break;

    default:
      std::cout << "Framebuffer status failure: " << status << std::endl;
      break;
  }
}

/* EOF */
