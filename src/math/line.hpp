/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_MATH_LINE_HPP
#define HEADER_WINDSTILLE_MATH_LINE_HPP

#include "math/vector2f.hpp"

/** */
class Line
{
private:
public:
  Vector2f p1;
  Vector2f p2;

  Line() : p1(), p2() {}

  Line(const Vector2f& p1,
       const Vector2f& p2);

  float length() const;

  /** Calculate if two lines intersec */
  bool intersect(const Line& line_b) const;

  /** Calculate if and where two lines intersect */
  bool intersect(const Line& line_b, float& ua, float& ub) const;

  /** Calculate if and where two lines intersect */
  bool intersect(const Line& line, Vector2f& colpos) const;

  /** Calculate the minimal distance between this line and the point p */
  float distance(const Vector2f& p) const;
};

#endif

/* EOF */
