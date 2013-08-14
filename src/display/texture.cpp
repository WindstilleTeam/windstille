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

#include "display/texture.hpp"

#include <stdint.h>
#include <sstream>
#include <stdexcept>

#include "math/rect.hpp"
#include "display/software_surface.hpp"
#include "display/opengl_state.hpp"
#include "display/texture_manager.hpp"
#include "display/assert_gl.hpp"
#include "util/util.hpp"

#pragma GCC diagnostic ignored "-Wold-style-cast"

TexturePtr
Texture::create(const Pathname& filename)
{
  return TextureManager::current()->get(filename);
}

TexturePtr
Texture::create(SoftwareSurfacePtr image, GLint format)
{
  return TexturePtr(new Texture(image, format));
}
  
TexturePtr
Texture::create(GLenum target, int width, int height, GLint format)
{
  return TexturePtr(new Texture(target, width, height, format));
}

Texture::Texture() :
  m_target(0),
  m_handle(0),
  m_width(0),
  m_height(0)
{
  glGenTextures(1, &m_handle);
  assert_gl("Texture::Texture()"); 
}

Texture::Texture(GLenum target, int width, int height, GLint format) :
  m_target(target),
  m_handle(0),
  m_width(width),
  m_height(height)
{
  if (!GLEW_ARB_texture_non_power_of_two)
  {
    if (!math::is_power_of_two(m_width) || !math::is_power_of_two(m_height))
    {
      std::cout  << "Texture::Texture(): texture dimensions have non power of two size: " << m_width << "x" << m_height;

      m_width  = math::round_to_power_of_two(m_width);
      m_height = math::round_to_power_of_two(m_height);
    }
  }

  glGenTextures(1, &m_handle);
  assert_gl("Texture::Texture()"); 

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexImage2D(target, 0, format, m_width, m_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, 0);

  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  assert_gl("Texture::Texture() 2"); 
}

Texture::Texture(SoftwareSurfacePtr image, GLint glformat) :
  m_target(GL_TEXTURE_2D),
  m_handle(0),
  m_width(image->get_width()),
  m_height(image->get_height())
{
  glGenTextures(1, &m_handle);
  assert_gl("Texture::Texture()"); 

  if (!GLEW_ARB_texture_non_power_of_two)
  {
    if (!math::is_power_of_two(image->get_width()) || !math::is_power_of_two(image->get_height()))
    {
      std::ostringstream str;
      str << "Texture::Texture(): image dimensions have non power of two size: " 
          << image->get_width() << "x" << image->get_height();
      throw std::runtime_error(str.str());
    }
  }

  if (image->get_bits_per_pixel() != 24 && image->get_bits_per_pixel() != 32)
  {
    throw std::runtime_error("image has not 24 or 32 bit color depth");
  }

  // FIXME: User SDL_ConvertSurface to bring images in the right format
  // SDL_ConvertSurface(bmp, screen->format, SDL_SWSURFACE);
  
  try 
  {
    GLint maxt;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);

    if(image->get_width() > maxt || image->get_height() > maxt)
    {
      throw std::runtime_error("Texture size not supported");
    }

    GLint sdl_format;

    if (image->get_format() == SoftwareSurface::RGB)
    {
      sdl_format = GL_RGB;
    }
    else if (image->get_format() == SoftwareSurface::RGBA)
    {
      sdl_format = GL_RGBA;
    }
    else
    {
      throw std::runtime_error("Texture: Image format not supported");
    }

    glBindTexture(GL_TEXTURE_2D, m_handle);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, image->get_pitch() / image->get_bytes_per_pixel());

    if (0)
    { // no mipmapping
      glTexImage2D(m_target, 0, glformat,
                   image->get_width(), image->get_height(), 0, sdl_format,
                   GL_UNSIGNED_BYTE, image->get_pixels());
        
      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    { // use mipmapping
      gluBuild2DMipmaps(m_target, glformat,
                        image->get_width(), image->get_height(), sdl_format,
                        GL_UNSIGNED_BYTE, image->get_pixels());
        
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    assert_gl("creating texture");

    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    assert_gl("setting texture parameters");
  } 
  catch(...)
  {
    throw;
  }
}

Texture::~Texture()
{
  glDeleteTextures(1, &m_handle);
}

int
Texture::get_width() const
{
  return m_width;
}

int
Texture::get_height() const
{
  return m_height;
}

GLuint
Texture::get_handle() const
{
  return m_handle;
}

void
Texture::put(SoftwareSurfacePtr image, const Rect& srcrect, int x, int y)
{
  GLint sdl_format;

  if (image->get_format() == SoftwareSurface::RGB)
  {
    sdl_format = GL_RGB;
  }
  else if (image->get_format() == SoftwareSurface::RGBA)
  {
    sdl_format = GL_RGBA;
  }
  else
  {
    throw std::runtime_error("Texture: SoftwareSurface format not supported");
  }

  glBindTexture(GL_TEXTURE_2D, m_handle);

  // FIXME: Add some checks here to make sure image has the right format 
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // FIXME: Does SDL always use 4?
  glPixelStorei(GL_UNPACK_ROW_LENGTH,
                image->get_pitch() / image->get_bytes_per_pixel());

  glTexSubImage2D(m_target, 0, x, y,
                  srcrect.get_width(), srcrect.get_height(), sdl_format, GL_UNSIGNED_BYTE,
                  static_cast<uint8_t*>(image->get_pixels())
                  + srcrect.top  * image->get_pitch()
                  + srcrect.left * image->get_bytes_per_pixel());
}

void
Texture::put(SoftwareSurfacePtr image, int x, int y)
{
  put(image, Rect(0, 0, image->get_width(), image->get_height()), x, y);
}

void
Texture::set_wrap(GLenum mode)
{
  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(m_target, GL_TEXTURE_WRAP_S, mode);
  glTexParameteri(m_target, GL_TEXTURE_WRAP_T, mode);
  glTexParameteri(m_target, GL_TEXTURE_WRAP_R, mode); // FIXME: only good for 3d textures?!
}

void
Texture::set_filter(GLenum mode)
{
  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, mode);
  glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, mode);
}

SoftwareSurfacePtr
Texture::get_software_surface() const
{
  glBindTexture(GL_TEXTURE_2D, m_handle);

  SoftwareSurfacePtr surface = SoftwareSurface::create(m_width, m_height);

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->get_pixels());

  return surface;
}

GLenum
Texture::get_target() const
{
  return m_target;
}

/* EOF */
