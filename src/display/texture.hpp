/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>,
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

#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <boost/shared_ptr.hpp>

class TextureImpl;

class Texture
{
public:
  /** Create a empty and invalid Texture object (similar to a
      NULL-pointer) */
  explicit Texture();

  /** Load a texture from file */
  explicit Texture(const std::string& filename);

  /**
   * Upload an SDL_Surface onto an OpenGL texture. The surface must have power
   * of 2 dimensions
   * */
  explicit Texture(SDL_Surface* image, GLint format = GL_RGBA);

  /** 
   * Create an empty Texture with the given dimensions
   */
  explicit Texture(GLenum target, int width, int height, GLint format = GL_RGBA);

  ~Texture();

  bool operator==(const Texture&) const;
  bool operator!=(const Texture&) const;

  int get_width() const;
  int get_height() const;

  /** Shortcut to set the texture into a mode where uv coordinates
      wrap around when they excede the [0,1] region */
  void set_wrap(GLenum mode);

  /** */
  void set_filter(GLenum mode);

  /** Uploads the given image to the given coordinates */
  void put(SDL_Surface* image, int x, int y);

  GLuint get_handle() const;
  
  /**
   * Return the target used by this texture, ie. GL_TEXTURE_2D or
   * GL_TEXTURE_RECTANGLE_ARB
   */ 
  GLenum get_target() const;

  /** true if the Texture is valid and usable, false if not */
  operator bool() const;

private:
  boost::shared_ptr<TextureImpl> impl;
};

#endif

/* EOF */
