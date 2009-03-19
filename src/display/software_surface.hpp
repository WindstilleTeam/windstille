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

#ifndef HEADER_SOFTWARE_SURFACE_HPP
#define HEADER_SOFTWARE_SURFACE_HPP

#include "SDL.h"
#include <boost/shared_ptr.hpp>
#include <string>

class SoftwareSurfaceImpl;

class SoftwareSurface
{
public:
  enum Format {
    RGB,
    RGBA
  };

  SoftwareSurface(const std::string& filename);
  SoftwareSurface(int width, int height, Format format = RGBA);
  ~SoftwareSurface();

  int   get_bytes_per_pixel() const;
  int   get_bits_per_pixel() const;
  int   get_width() const;
  int   get_pitch() const;
  int   get_height() const;
  void* get_pixels() const;

  SDL_Surface* get_surface() const;

private:
  boost::shared_ptr<SoftwareSurfaceImpl> impl;
};

#endif

/* EOF */
