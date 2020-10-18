/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_COLOR_HPP
#define HEADER_WINDSTILLE_DISPLAY_COLOR_HPP

#include <stdint.h>
#include <iosfwd>

class RGBAf
{
public:
  RGBAf()
    : r(0), g(0), b(0), a(1.0)
  { }
  RGBAf(float r_, float g_, float b_, float a_ = 1.0)
    : r(r_), g(g_), b(b_), a(a_)
  { }

  inline uint8_t r8() const { return static_cast<uint8_t>(255.0f * r); }
  inline uint8_t g8() const { return static_cast<uint8_t>(255.0f * g); }
  inline uint8_t b8() const { return static_cast<uint8_t>(255.0f * b); }
  inline uint8_t a8() const { return static_cast<uint8_t>(255.0f * a); }

public:
  float r;
  float g;
  float b;
  float a;
};

std::ostream& operator<<(std::ostream& out, const RGBAf& color);

#endif

/* EOF */
