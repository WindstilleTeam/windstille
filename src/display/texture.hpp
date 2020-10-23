/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>,
**                          Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_TEXTURE_HPP
#define HEADER_WINDSTILLE_DISPLAY_TEXTURE_HPP

#include <string>
#include <GL/glew.h>
#include <memory>

#include <geom/rect.hpp>
#include <surf/fwd.hpp>

#include "display/software_surface.hpp"

class TextureImpl;

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class Texture
{
public:
  /** Upload an SoftwareSurface onto an OpenGL texture. */
  static TexturePtr create(SoftwareSurface const& image, GLint format = GL_RGBA);

  /** Create an empty Texture with the given dimensions */
  static TexturePtr create(GLenum target, geom::isize const& size, GLint format = GL_RGBA);

private:
  Texture();
  Texture(SoftwareSurface const& image, GLint format = GL_RGBA);
  Texture(GLenum target, geom::isize const& size, GLint format = GL_RGBA);

public:
  ~Texture();

  int get_width() const;
  int get_height() const;
  geom::isize get_size() const;

  /** Shortcut to set the texture into a mode where uv coordinates
      wrap around when they excede the [0,1] region */
  void set_wrap(GLenum mode);

  /** */
  void set_filter(GLenum mode);

  /** Uploads the given image to the given coordinates */
  void put(SoftwareSurface const& image, int x, int y);

  /** Uploads the given subsection \a srcrect of \a image to the given
      coordinates */
  void put(SoftwareSurface const& image, const geom::irect& srcrect, int x, int y);

  GLuint get_handle() const;

  /**
   * Return the target used by this texture, ie. GL_TEXTURE_2D
   */
  GLenum get_target() const;

  SoftwareSurface get_software_surface() const;

private:
  GLenum m_target;
  GLuint m_handle;
  geom::isize m_size;
};

#endif

/* EOF */
