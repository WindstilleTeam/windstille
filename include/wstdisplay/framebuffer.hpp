// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_DISPLAY_FRAMEBUFFER_HPP
#define HEADER_WINDSTILLE_DISPLAY_FRAMEBUFFER_HPP

#include <geom/size.hpp>

#include "texture.hpp"
#include "renderbuffer.hpp"

namespace wstdisplay {

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;

class Framebuffer
{
public:
  static FramebufferPtr create_with_texture(GLenum target, geom::isize const& size, int multisample = 0);
  static FramebufferPtr create(geom::isize const& size, int multisample = 0);
  static FramebufferPtr create_hdr(geom::isize const& size, int multisample = 0);

public:
  ~Framebuffer();

  int get_width()  const;
  int get_height() const;
  geom::isize get_size() const;
  TexturePtr get_texture();

  GLuint get_handle() const;

  /** @param mask GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT
      @param filter GL_NEAREST or GL_LINEAR. */
  void blit(geom::irect const& srcrect, geom::irect const& dstrect,
            GLbitfield mask, GLenum filter);

private:
  Framebuffer();
  void check_completness();
  void create_internal(GLenum format, geom::isize const& size, int multisample);
  void create_with_texture_internal(GLenum target, geom::isize const& size, int multisample);

private:
  GLuint m_handle;
  geom::isize   m_size;

  TexturePtr m_texture;
  RenderbufferPtr m_color_buffer;
  RenderbufferPtr m_depth_stencil_buffer;
};

} // namespace wstdisplay

#endif

/* EOF */
