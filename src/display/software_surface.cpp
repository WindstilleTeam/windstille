/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <iostream>
#include <fmt/format.h>
#include <png.h>
#include <errno.h>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>

#include <geom/rect.hpp>

#include "display/software_surface.hpp"

SoftwareSurfacePtr
SoftwareSurface::create(std::filesystem::path const& filename)
{
  return SoftwareSurfacePtr(new SoftwareSurface(filename));
}

SoftwareSurfacePtr
SoftwareSurface::create(Format format, int width, int height, Color const& color)
{
  SoftwareSurfacePtr sur(new SoftwareSurface(width, height, format));

  switch (format)
  {
    case RGBA:
      SDL_FillRect(sur->m_surface, nullptr,
                   SDL_MapRGBA(sur->m_surface->format,
                               static_cast<uint8_t>(color.r * 255),
                               static_cast<uint8_t>(color.g * 255),
                               static_cast<uint8_t>(color.b * 255),
                               static_cast<uint8_t>(color.a * 255)));
      break;

    case RGB:
      SDL_FillRect(sur->m_surface, nullptr,
                   SDL_MapRGB(sur->m_surface->format,
                               static_cast<uint8_t>(color.r * 255),
                               static_cast<uint8_t>(color.g * 255),
                               static_cast<uint8_t>(color.b * 255)));
      break;
  }

  return sur;
}

SoftwareSurface::SoftwareSurface(std::filesystem::path const& filename) :
  m_surface(nullptr),
  m_format(RGBA)
{
  m_surface = IMG_Load(filename.c_str());

  if (!m_surface)
  {
    std::ostringstream str;
    str << "SoftwareSurface: Couldn't load: " << filename << std::endl;
    throw std::runtime_error(str.str());
  }
  else
  {
    SDL_SetSurfaceBlendMode(m_surface, SDL_BLENDMODE_NONE);

    if (m_surface->format->BytesPerPixel == 4)
    { // convert image into standard format
      m_format = RGBA;

      if ((std::endian::native == std::endian::little &&
           !(m_surface->format->Rmask == 0x000000ff &&
             m_surface->format->Gmask == 0x0000ff00 &&
             m_surface->format->Bmask == 0x00ff0000 &&
             m_surface->format->Amask == 0xff000000)) ||
          (std::endian::native == std::endian::big &&
           !(m_surface->format->Rmask == 0xff000000 &&
             m_surface->format->Gmask == 0x00ff0000 &&
             m_surface->format->Bmask == 0x0000ff00 &&
             m_surface->format->Amask == 0x000000ff)))
      {
        std::cout << "XXX Doing conversation RGBA: " << filename << std::endl;
        std::cout << "    rmask: " << fmt::format("{:08x} {:08x} {:08x} {:08x}",
                                                  m_surface->format->Rmask,
                                                  m_surface->format->Gmask,
                                                  m_surface->format->Bmask,
                                                  m_surface->format->Amask) << std::endl;

        SDL_Surface* tmp_surface;
        if (std::endian::native == std::endian::little)
        {
          tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                             m_surface->w, m_surface->h, 32,
                                             0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        }
        else
        {
          tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                             m_surface->w, m_surface->h, 32,
                                             0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        }

        SDL_BlitSurface(m_surface, nullptr, tmp_surface, nullptr);

        std::swap(m_surface, tmp_surface);
        SDL_FreeSurface(tmp_surface);
      }
    }
    else if (m_surface->format->BytesPerPixel == 3)
    {
      m_format = RGB;

      if ((std::endian::native == std::endian::little &&
           !(m_surface->format->Rmask == 0x0000ff &&
             m_surface->format->Gmask == 0x00ff00 &&
             m_surface->format->Bmask == 0xff0000 &&
             m_surface->format->Amask == 0x000000)) ||
          (std::endian::native == std::endian::big &&
           !(m_surface->format->Rmask == 0xff0000 &&
             m_surface->format->Gmask == 0x00ff00 &&
             m_surface->format->Bmask == 0x0000ff &&
             m_surface->format->Amask == 0x000000)))
      {
        std::cout << "XXX Doing conversation RGB: " << filename << std::endl;
        std::cout << "    rmask: " << fmt::format("{:08x} {:08x} {:08x} {:08x}",
                                                  m_surface->format->Rmask,
                                                  m_surface->format->Gmask,
                                                  m_surface->format->Bmask,
                                                  m_surface->format->Amask) << std::endl;


        SDL_Surface* tmp_surface;
        if (std::endian::native == std::endian::little)
        {
          tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                             m_surface->w, m_surface->h, 24,
                                             0x0000ff, 0x00ff00, 0xff0000, 0);
        }
        else
        {
          tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                             m_surface->w, m_surface->h, 24,
                                             0xff000, 0x00ff00, 0x0000ff, 0);
        }

        SDL_BlitSurface(m_surface, nullptr, tmp_surface, nullptr);

        std::swap(m_surface, tmp_surface);
        SDL_FreeSurface(tmp_surface);
      }
    }
    else
    {
      std::ostringstream str;
      str << "SoftwareSurface: unknown bytesPerPixel: " << m_surface->format->BytesPerPixel << std::endl;
      throw std::runtime_error(str.str());
    }

    assert(!SDL_MUSTLOCK(m_surface));
  }
}

SoftwareSurface::SoftwareSurface(int width, int height, Format format) :
  m_surface(nullptr),
  m_format(format)
{
  assert(format == RGBA);

  if (std::endian::native == std::endian::big)
  {
    m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                     width, height, 32,
                                     0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  }
  else
  {
    m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                     width, height, 32,
                                     0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
  }

  assert(!SDL_MUSTLOCK(m_surface));
}

SoftwareSurface::~SoftwareSurface()
{
  SDL_FreeSurface(m_surface);
}

int
SoftwareSurface::get_bytes_per_pixel() const
{
  return m_surface->format->BytesPerPixel;
}

int
SoftwareSurface::get_bits_per_pixel() const
{
  return m_surface->format->BitsPerPixel;
}

int
SoftwareSurface::get_width() const
{
  return m_surface->w;
}

int
SoftwareSurface::get_height() const
{
  return m_surface->h;
}

geom::isize
SoftwareSurface::get_size() const
{
  return geom::isize(m_surface->w,
              m_surface->h);
}

int
SoftwareSurface::get_pitch() const
{
  return m_surface->pitch;
}

void*
SoftwareSurface::get_pixels() const
{
  return m_surface->pixels;
}

SDL_Surface*
SoftwareSurface::get_surface() const
{
  return m_surface;
}

void
SoftwareSurface::blit(SoftwareSurfacePtr dst, int x, int y) const
{
  SDL_Rect dst_rect;
  dst_rect.x = static_cast<Sint16>(x);
  dst_rect.y = static_cast<Sint16>(y);

  SDL_BlitSurface(m_surface, nullptr, dst->m_surface, &dst_rect);
}

void
SoftwareSurface::blit(const geom::irect& src_rect_, SoftwareSurfacePtr dst, int x, int y) const
{
  SDL_Rect src_rect;
  src_rect.x = static_cast<Sint16>(src_rect_.left());
  src_rect.y = static_cast<Sint16>(src_rect_.top());
  src_rect.w = static_cast<Sint16>(src_rect_.width());
  src_rect.h = static_cast<Sint16>(src_rect_.height());

  SDL_Rect dst_rect;
  dst_rect.x = static_cast<Sint16>(x);
  dst_rect.y = static_cast<Sint16>(y);

  SDL_BlitSurface(m_surface, &src_rect, dst->m_surface, &dst_rect);
}

bool
SoftwareSurface::is_at(int x, int y) const
{
  if (x >= 0 && x < m_surface->w &&
      y >= 0 && y < m_surface->h)
  {
    if (get_bits_per_pixel() == 32)
    {
      uint8_t* pixels = static_cast<uint8_t*>(m_surface->pixels);

      return pixels[y * m_surface->pitch + x*4 + 3] > 128;
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
  if (get_bytes_per_pixel() != 4 &&
      get_bytes_per_pixel() != 3)
  {
    std::cout << "SoftwareSurface::save_png(): Unsupported pixel format" << std::endl;
  }
  else
  {
    FILE* fp = fopen(filename.c_str(), "w");

    if (!fp)
    {
      std::cout << "SoftwareSurface::save_png(): error: couldn't save SoftwareSurface: " << strerror(errno) << std::endl;
      return;
    }
    else
    {
      int pitch   = get_pitch();
      png_structp png_ptr;
      png_infop   info_ptr;
      png_byte* pixels = static_cast<png_byte*>(get_pixels());

      png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
      info_ptr = png_create_info_struct(png_ptr);

      png_init_io(png_ptr, fp);

      png_set_IHDR(png_ptr, info_ptr,
                   get_width(), get_height(), 8 /* bitdepth */,
                   (get_bytes_per_pixel() == 4) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_BASE,
                   PNG_FILTER_TYPE_BASE);

      png_set_compression_level(png_ptr, 6);
      png_write_info(png_ptr, info_ptr);

      std::vector<png_bytep> row_pointers(get_height());

      for (int y = 0; y < get_height(); ++y)
      {
        row_pointers[y] = (pixels + (y * pitch));
      }

      png_write_image(png_ptr, row_pointers.data());

      png_write_end(png_ptr, info_ptr);

      fclose(fp);
    }
  }
}

/* EOF */
