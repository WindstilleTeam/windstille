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

#ifndef HEADER_WINDSTILLE_MATH_QUAD_HPP
#define HEADER_WINDSTILLE_MATH_QUAD_HPP

#include <math.h>
#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>

#include "math/math.hpp"
#include "vector2f.hpp"
#include "math/rect.hpp"

class Quad
{
private:
public:
  Vector2f p1;
  Vector2f p2;
  Vector2f p3;
  Vector2f p4;

  Quad()
    : p1(),
      p2(),
      p3(),
      p4()
  {}

  Quad(const Rectf& rect)
    : p1(rect.left, rect.top),
      p2(rect.right, rect.top),
      p3(rect.right, rect.bottom),
      p4(rect.left, rect.bottom)
  {}

  Quad(float x1, float y1,
       float x2, float y2)
    : p1(x1, y1),
      p2(x2, y1),
      p3(x2, y2),
      p4(x1, y2)
  {}

  Quad(const Vector2f& p1_,
       const Vector2f& p2_,
       const Vector2f& p3_,
       const Vector2f& p4_)
    : p1(p1_),
      p2(p2_),
      p3(p3_),
      p4(p4_)
  {}

  Rectf get_bounding_box() const
  {
    return Rectf(math::min(math::min(math::min(p1.x, p2.x), p3.x), p4.x),
                 math::min(math::min(math::min(p1.y, p2.y), p3.y), p4.y),
                 math::max(math::max(math::max(p1.x, p2.x), p3.x), p4.x),
                 math::max(math::max(math::max(p1.y, p2.y), p3.y), p4.y));
  }

  void rotate(float rad)
  {
    Vector2f center((p1.x + p2.x + p3.x + p4.x) / 4.0f,
                    (p1.y + p2.y + p3.y + p4.y) / 4.0f);

    p1 = center + glm::rotate(p1 - center, rad);
    p2 = center + glm::rotate(p2 - center, rad);
    p3 = center + glm::rotate(p3 - center, rad);
    p4 = center + glm::rotate(p4 - center, rad);
  }
};

#endif

/* EOF */
