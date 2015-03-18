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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_HPP

#include "display/texture.hpp"
#include "math/rect.hpp"
#include "util/pathname.hpp"

class SurfaceDrawingParameters;
class Surface;
typedef boost::shared_ptr<Surface> SurfacePtr;

/**
 * Surface class. This class basically holds a reference to an opengl texture
 * along with texture coordinates that specify a rectangle on that texture.
 * Several surface may share a single texture (but have different texture
 * coordinates then)
 */
class Surface
{
public:
  static SurfacePtr create(const Pathname& filename);

  /**
   * Create a new Surface object from a Texture
   * @param texture The Texture to use
   * @param uv The rectangle of the texture to use in (0-1) notation
   * @param width  Width of the surface on the screen
   * @param height Height of the surface on the screen
   */
  static SurfacePtr create(TexturePtr texture, const Rectf& uv, const Sizef& size);
  static SurfacePtr create(int width, int height);

private:
  Surface(TexturePtr texture, const Rectf& uv, const Sizef& size);
  Surface(int width, int height);

public:
  ~Surface();

  float get_width()  const;
  float get_height() const;
  Sizef get_size() const;

  TexturePtr get_texture() const;

  /** Returns texture coordinates for the Surface rectangle */
  Rectf get_uv() const;

  void draw(const Vector2f& pos) const;
  void draw(const SurfaceDrawingParameters& params) const;

private:
  /**
   * Texture on which the surface is located
   */
  TexturePtr m_texture;

  /**
   * uv coordinates of the Surface in [0,1] range
   */
  Rectf m_uv;

  /**
   * The size of the Surface in pixels
   */
  Sizef m_size;
};

#endif

/* EOF */
