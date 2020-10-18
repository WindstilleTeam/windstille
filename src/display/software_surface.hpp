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

#ifndef HEADER_WINDSTILLE_DISPLAY_SOFTWARE_SURFACE_HPP
#define HEADER_WINDSTILLE_DISPLAY_SOFTWARE_SURFACE_HPP

#include <filesystem>
#include <memory>

#include <geom/size.hpp>
#include <geom/rect.hpp>

#include "display/color.hpp"

typedef struct SDL_Surface SDL_Surface;

class SoftwareSurface;
typedef std::shared_ptr<SoftwareSurface> SoftwareSurfacePtr;

class SoftwareSurface
{
public:
  enum Format {
    RGB,
    RGBA
  };

public:
  static SoftwareSurfacePtr create(std::filesystem::path const& filename);
  static SoftwareSurfacePtr create(Format format, geom::isize const& size, RGBAf const& color = RGBAf(0.0f, 0.0f, 0.0f, 0.0f));

private:
  explicit SoftwareSurface(std::filesystem::path const& filename);
  SoftwareSurface(geom::isize const& size, Format format = RGBA);

public:
  ~SoftwareSurface();

  int   get_bytes_per_pixel() const;
  int   get_bits_per_pixel() const;
  int   get_width() const;
  int   get_pitch() const;
  int   get_height() const;
  geom::isize  get_size() const;
  void* get_pixels() const;

  void blit(SoftwareSurfacePtr dst, int x, int y) const;
  void blit(const geom::irect& src_rect, SoftwareSurfacePtr dst, int x, int y) const;

  void save_png(const std::string& filename) const;

  SDL_Surface* get_surface() const;

  bool is_at(int x, int y) const;

  Format get_format() const { return m_format; }

private:
  SDL_Surface* m_surface;
  Format m_format;

private:
  SoftwareSurface(const SoftwareSurface&);
  SoftwareSurface& operator=(const SoftwareSurface&);
};

#endif

/* EOF */
