/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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
#include <assert.h>
#include <iostream>

#include <geom/size.hpp>

#include "display/assert_gl.hpp"
#include "display/renderbuffer.hpp"
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
  assert_gl();
  glGenFramebuffers(1, &m_handle);
  assert_gl();
}

Framebuffer::~Framebuffer()
{
  glDeleteFramebuffers(1, &m_handle);
}

TexturePtr
Framebuffer::get_texture()
{
  assert(m_texture);
  return m_texture;
}

int
Framebuffer::get_width()  const
{
  return m_size.width();
}

int
Framebuffer::get_height() const
{
  return m_size.height();
}

GLuint
Framebuffer::get_handle() const
{
  return m_handle;

}

void
Framebuffer::create_with_texture_internal(GLenum target, int width, int height, int multisample)
{
  assert_gl();

  m_size = geom::isize(width, height);
  m_texture = Texture::create(target, width, height);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, width, height, multisample);

  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

  // bind texture and renderbuffers to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->get_target(), m_texture->get_handle(), 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());

  assert_gl();

  check_completness();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
Framebuffer::create_internal(GLenum format, int width, int height, int multisample)
{
  assert_gl();

  m_size = geom::isize(width, height);
  m_color_buffer = Renderbuffer::create(format, width, height, multisample);
  m_depth_stencil_buffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, width, height, multisample);

  // FIXME: Should use push/pop_framebuffer instead, but don't have pointer to Framebuffer here
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  GL_RENDERBUFFER, m_color_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_buffer->get_handle());
  assert_gl();

  check_completness();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert_gl();
}

void
Framebuffer::check_completness()
{
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(status)
  {
    case GL_FRAMEBUFFER_COMPLETE:
      std::cout << "Framebuffer ok" << std::endl;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n";
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      std::cout << "GL_FRAMEBUFFER_UNSUPPORTED\n";
      break;
    default:
      std::cout << "Framebuffer status failure: " << status << std::endl;
      break;
  }
}

/* EOF */
