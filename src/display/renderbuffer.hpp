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

#ifndef HEADER_WINDSTILLE_DISPLAY_RENDER_BUFFER_HPP
#define HEADER_WINDSTILLE_DISPLAY_RENDER_BUFFER_HPP

#include <memory>
#include <geom/size.hpp>

class Renderbuffer;
typedef std::shared_ptr<Renderbuffer> RenderbufferPtr;

class Renderbuffer
{
public:
  static RenderbufferPtr create(GLenum format, geom::isize const& size, int multisample = 0);

public:
  ~Renderbuffer();

  GLuint get_handle() const;

private:
  /**
   * @param width  Width of the Renderbuffer
   * @param height Height of the Renderbuffer
   * @param format GL_DEPTH_COMPONENT24, GL_STENCIL_INDEX, ...
   */
  Renderbuffer(GLenum format, geom::isize const& size, int multisample = 0);

private:
  GLuint m_handle;
};

#endif

/* EOF */
