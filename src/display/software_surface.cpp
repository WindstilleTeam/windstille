/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <boost/scoped_array.hpp>
#include <png.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>

#include "util/pathname.hpp"
#include "math/rect.hpp"
#include "physfs/physfs_sdl.hpp"
#include "software_surface.hpp"

class SoftwareSurfaceImpl
{
public:
  SDL_Surface* surface;
};

SoftwareSurface::SoftwareSurface(const Pathname& filename)
  : impl(new SoftwareSurfaceImpl())
{
  impl->surface = IMG_Load(filename.get_sys_path().c_str());

  if (!impl->surface)
    {
      std::ostringstream str;
      str << "SoftwareSurface: Couldn't load: " << filename << std::endl;
      throw std::runtime_error(str.str());
    }
  else
    {
      SDL_SetAlpha(impl->surface, 0, 0);

      assert(!SDL_MUSTLOCK(impl->surface));
    }
}

SoftwareSurface::SoftwareSurface(int width, int height, Format format)
  : impl(new SoftwareSurfaceImpl())
{
  assert(format == RGBA);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  impl->surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                       width, height, 32,
                                       0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  impl->surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                       width, height, 32,
                                       0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  SDL_SetAlpha(impl->surface, 0, 0);

  assert(!SDL_MUSTLOCK(impl->surface));
}

SoftwareSurface::~SoftwareSurface()
{
}

int
SoftwareSurface::get_bytes_per_pixel() const
{
  return impl->surface->format->BytesPerPixel;
}

int
SoftwareSurface::get_bits_per_pixel() const
{
  return impl->surface->format->BitsPerPixel;
}

int
SoftwareSurface::get_width() const
{
  return impl->surface->w;
}

int
SoftwareSurface::get_height() const
{
  return impl->surface->h;
}

Size
SoftwareSurface::get_size() const
{
  return Size(impl->surface->w, 
              impl->surface->h);
}

int
SoftwareSurface::get_pitch() const
{
  return impl->surface->pitch;
}

void*
SoftwareSurface::get_pixels() const
{
  return impl->surface->pixels;
}

SDL_Surface*
SoftwareSurface::get_surface() const
{
  return impl->surface;
}

void
SoftwareSurface::blit(SoftwareSurface& dst, int x, int y) const
{
  SDL_Rect dst_rect;
  dst_rect.x = x;
  dst_rect.y = y;

  SDL_BlitSurface(impl->surface, 0, dst.impl->surface, &dst_rect);
}

void
SoftwareSurface::blit(const Rect& src_rect_, SoftwareSurface& dst, int x, int y) const
{
  SDL_Rect src_rect;
  src_rect.x = src_rect_.left;
  src_rect.y = src_rect_.top;
  src_rect.w = src_rect_.get_width();
  src_rect.h = src_rect_.get_height();

  SDL_Rect dst_rect;
  dst_rect.x = x;
  dst_rect.y = y;
  
  SDL_BlitSurface(impl->surface, &src_rect, dst.impl->surface, &dst_rect);
}

bool
SoftwareSurface::is_at(int x, int y) const
{
  if (x >= 0 && x < impl->surface->w &&
      y >= 0 && y < impl->surface->h)
    {
      if (get_bits_per_pixel() == 32)
        {
          uint8_t* pixels = (uint8_t*)impl->surface->pixels;
          
          return pixels[y * impl->surface->pitch + x*4 + 3] > 128;
        }
      else
        {
          return true;
        }
    }
  else
    {
      return false;
    }
}

void
SoftwareSurface::save_png(const std::string& filename) const
{
  // FIXME: This is upside down  

  if (get_bytes_per_pixel() != 4 &&
      get_bytes_per_pixel() != 3)
    {
      std::cout << "SoftwareSurface::save_png(): Unsupported pixel format" << std::endl;
      return;
    }

  FILE* fp = fopen(filename.c_str(), "w");

  if (!fp)
    {
      std::cout << "Error: Couldn't save screenshot: " << strerror(errno) << std::endl;
      return;
    }
  else
    {
      int pitch   = get_pitch();
      png_structp png_ptr;
      png_infop   info_ptr;
      png_byte* pixels = static_cast<png_byte*>(get_pixels());

      png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      info_ptr = png_create_info_struct(png_ptr);

      png_init_io(png_ptr, fp);

      png_set_IHDR(png_ptr, info_ptr, 
                   get_width(), get_height(), 8 /* bitdepth */,
                   (get_bytes_per_pixel() == 32) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE, 
                   PNG_COMPRESSION_TYPE_BASE, 
                   PNG_FILTER_TYPE_BASE);
      
      png_set_compression_level(png_ptr, 6);
      png_write_info(png_ptr, info_ptr);

      boost::scoped_array<png_bytep> row_pointers(new png_bytep[get_height()]);
   
      // generate row pointers
      for (int k = 0; k < get_height(); k++)
        row_pointers[k] = reinterpret_cast<png_byte*>(pixels + ((get_height() - k - 1) * pitch));

      png_write_image(png_ptr, row_pointers.get());

      png_write_end(png_ptr, info_ptr);

      fclose(fp);
    }
}

/* EOF */
