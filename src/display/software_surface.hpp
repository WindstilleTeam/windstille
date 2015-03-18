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

#ifndef HEADER_WINDSTILLE_DISPLAY_SOFTWARE_SURFACE_HPP
#define HEADER_WINDSTILLE_DISPLAY_SOFTWARE_SURFACE_HPP

#include <boost/shared_ptr.hpp>

#include "util/pathname.hpp"
#include "math/size.hpp"

typedef struct SDL_Surface SDL_Surface;


class Rect;
class SoftwareSurface;
typedef boost::shared_ptr<SoftwareSurface> SoftwareSurfacePtr;

class SoftwareSurface
{
public:
  enum Format {
    RGB,
    RGBA
  };

public:
  static SoftwareSurfacePtr create(const Pathname& filename);
  static SoftwareSurfacePtr create(int width, int height, Format format = RGBA);

private:
  explicit SoftwareSurface(const Pathname& filename);
  SoftwareSurface(int width, int height, Format format = RGBA);

public:
  ~SoftwareSurface();

  int   get_bytes_per_pixel() const;
  int   get_bits_per_pixel() const;
  int   get_width() const;
  int   get_pitch() const;
  int   get_height() const;
  Size  get_size() const;
  void* get_pixels() const;

  void blit(SoftwareSurfacePtr dst, int x, int y) const;
  void blit(const Rect& src_rect, SoftwareSurfacePtr dst, int x, int y) const;

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
