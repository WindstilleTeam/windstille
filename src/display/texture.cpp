/*  $Id: windstille_main.hpp 752 2005-07-25 10:00:44Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>,
**                          Matthias Braun <matze@braunis.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <stdexcept>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "texture.hpp"
#include "display/opengl_state.hpp"
#include "texture_manager.hpp"
#include "util.hpp"

class TextureImpl
{
public:
  GLenum target;
  GLuint handle;
  int    width;
  int    height;

  TextureImpl()
  {
    glGenTextures(1, &handle);
    assert_gl("creating texture handle."); 
  }

  ~TextureImpl()
  {
    glDeleteTextures(1, &handle);
  }
};

Texture::Texture()
{
}

Texture::Texture(const std::string& filename)
{
  *this = texture_manager->get(filename);

  std::cout << "Filename: " << filename << " -> " << get_handle() << std::endl;
}

Texture::Texture(GLenum target, int width, int height, GLint format)
  : impl(new TextureImpl())
{
  impl->target = target;
  impl->width  = width;
  impl->height = height;

  OpenGLState state;
  state.bind_texture(*this);
  state.activate();

  glTexImage2D(target, 0, format, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, 0);

  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP);
}

static inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}

Texture::Texture(SDL_Surface* image, GLint glformat)
  : impl(new TextureImpl())
{
  impl->target = GL_TEXTURE_2D;
  impl->width  = image->w;
  impl->height = image->h;

  const SDL_PixelFormat* format = image->format;

  if(!is_power_of_2(image->w) || !is_power_of_2(image->h))
    throw std::runtime_error("image has no power of 2 size");

  if(format->BitsPerPixel != 24 && format->BitsPerPixel != 32)
    throw std::runtime_error("image has not 24 or 32 bit color depth");

  // FIXME: User SDL_ConvertSurface to bring images in the right format
  // SDL_ConvertSurface(bmp, screen->format, SDL_SWSURFACE);
  
  try 
    {
      GLint maxt;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);
      if(image->w > maxt || image->h > maxt)
        {
          throw std::runtime_error("Texture size not supported");
        }

      GLint sdl_format;
      if (format->BytesPerPixel == 3)
        {
          sdl_format = GL_RGB;
        }
      else if (format->BytesPerPixel == 4)
        {
          sdl_format = GL_RGBA;
        }
      else
        {
          throw std::runtime_error("Texture: Image format not supported");
        }

      OpenGLState state;
      state.bind_texture(*this);
      state.activate();

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, image->pitch/format->BytesPerPixel);
      glTexImage2D(impl->target, 0, glformat,
                   image->w, image->h, 0, sdl_format,
                   GL_UNSIGNED_BYTE, image->pixels);

      assert_gl("creating texture");

      glTexParameteri(impl->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(impl->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(impl->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(impl->target, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(impl->target, GL_TEXTURE_WRAP_R, GL_CLAMP);

      assert_gl("setting texture parameters");
    } 
  catch(...)
    {
      throw;
    }
}

Texture::~Texture()
{
}

int
Texture::get_width() const
{
  return impl->width;
}

int
Texture::get_height() const
{
  return impl->height;
}

GLuint
Texture::get_handle() const
{
  return impl->handle;
}

void
Texture::put(SDL_Surface* image, int x, int y)
{
  GLint sdl_format;
  if (image->format->BytesPerPixel == 3)
    {
      sdl_format = GL_RGB;
    }
  else if (image->format->BytesPerPixel == 4)
    {
      sdl_format = GL_RGBA;
    }
  else
    {
      throw std::runtime_error("Texture: Image format not supported");
    }

  OpenGLState state;
  state.bind_texture(*this);
  state.activate();

  // FIXME: Add some checks here to make sure image has the right format 
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH,
                image->pitch / image->format->BytesPerPixel);

  glTexSubImage2D(impl->target, 0, x, y,
                  image->w, image->h, sdl_format, GL_UNSIGNED_BYTE,
                  image->pixels);
}

void
Texture::set_wrap(GLenum mode)
{
  OpenGLState state;
  state.bind_texture(*this);
  state.activate();

  glTexParameteri(impl->target, GL_TEXTURE_WRAP_S, mode);
  glTexParameteri(impl->target, GL_TEXTURE_WRAP_T, mode);
  glTexParameteri(impl->target, GL_TEXTURE_WRAP_R, mode); // FIXME: only good for 3d textures?!
}

void
Texture::set_filter(GLenum mode)
{
  OpenGLState state;
  state.bind_texture(*this);
  state.activate();

  glTexParameteri(impl->target, GL_TEXTURE_MIN_FILTER, mode);
  glTexParameteri(impl->target, GL_TEXTURE_MAG_FILTER, mode);
}

Texture::operator bool() const
{
  return !impl.is_null();
}

bool
Texture::operator==(const Texture& other) const
{
  return impl.get() == other.impl.get();
}

bool
Texture::operator!=(const Texture& other) const
{
  return impl.get() != other.impl.get();
}

GLenum
Texture::get_target() const
{
  return impl->target;
}

/* EOF */
