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

#ifndef HEADER_WINDSTILLE_DISPLAY_FRAMEBUFFER_HPP
#define HEADER_WINDSTILLE_DISPLAY_FRAMEBUFFER_HPP

#include "math/size.hpp"
#include "display/texture.hpp"
#include "display/renderbuffer.hpp"

class Framebuffer;
typedef boost::shared_ptr<Framebuffer> FramebufferPtr;

class Framebuffer
{
public:
  static FramebufferPtr create_with_texture(GLenum target, int width, int height, int multisample = 0);
  static FramebufferPtr create(int width, int height, int multisample = 0);
  static FramebufferPtr create_hdr(int width, int height, int multisample = 0);

public:  
  ~Framebuffer();

  int get_width()  const;
  int get_height() const;
  Texture get_texture();

  GLuint get_handle() const;

private:
  Framebuffer();
  void check_completness();
  void create_internal(GLenum format, int width, int height, int multisample);
  void create_with_texture_internal(GLenum target, int width, int height, int multisample);

private:
  GLuint m_handle;
  Size   m_size;
  
  Texture m_texture;
  RenderbufferPtr m_color_buffer;
  RenderbufferPtr m_depth_stencil_buffer;
};

#endif

/* EOF */
