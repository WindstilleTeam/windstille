/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

//! clanCore="Math"
//! header=core.h

#ifndef HEADER_WINDSTILLE_MATH_POINT_HPP
#define HEADER_WINDSTILLE_MATH_POINT_HPP

#include <cmath>

#include "vector2f.hpp"

//: 2D (x,y) point structure.
//- !group=Core/Math!
//- !header=core.h!
class Point
{
  // Construction:
public:
  //: Constructs a point.
  //param x: Initial x value.
  //param y: Initial y value.
  //param p: Point to use for initial values.
  Point()
    : x(0), y(0)
  {}

  Point(int x_, int y_)
    : x(x_),
      y(y_)
  {}

  constexpr Point(const Point &p) = default;
  explicit Point(const Vector2f& p);

  // Operations:
public:
  Point& operator=(Point const& p) = default;

  //: Return a rotated version of this point.
  //param hotspot: The point around which to rotate.
  //param angle: The amount of degrees to rotate by, clockwise.
  Point rotate(
    const Point &hotspot,
    float angle) const
  {
    //Move the hotspot to 0,0
    Point r(x - hotspot.x, y - hotspot.y);

    //Do some Grumbel voodoo.

    // Because MSVC sucks ass wrt standards compliance, it gets it own special function calls
    const float c = static_cast<float>(std::sqrt(static_cast<float>(r.x)*static_cast<float>(r.x) +
                                                 static_cast<float>(r.y)*static_cast<float>(r.y)));
    const float nw = static_cast<float>(std::atan2(static_cast<float>(r.y), static_cast<float>(r.x)) + ((angle + 180) * M_PI / 180));
    r.x = static_cast<int>((std::sin(1.5 * M_PI - nw) * c) + 0.5) + hotspot.x;
    r.y = -static_cast<int>((std::sin(nw) * c) + 0.5) + hotspot.y;

    return r;
  }

  //: Return the distance to another point.
  //param Point &p: The other point.
  int distance( const Point &p ) const
  {
    return int(std::sqrt(double((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y))) + 0.5f);
  }

  //: Translate point.
  Point &operator+=(const Point &p)
  { x += p.x; y += p.y; return *this; }

  //: Translate point negatively.
  Point &operator-=(const Point &p)
  { x -= p.x; y -= p.y; return *this; }

  //: Point + Point operator.
  Point operator+(const Point &p) const
  { return Point(x + p.x, y + p.y); }

  //: Point - Point operator.
  Point operator-(const Point &p) const
  { return Point(x - p.x, y - p.y); }

  //: Point == Point operator (deep compare)
  bool operator==(const Point &p) const
  { return (x == p.x) && (y == p.y); }

  //: Point != Point operator (deep compare)
  bool operator!=(const Point &p) const
  { return (x != p.x) || (y != p.y); }

  // Attributes:
public:
  //: X coordinate.
  int x;

  //: Y coordinate.
  int y;
};

inline Point::Point(const Vector2f& p)
  : x(static_cast<int>(p.x)),
    y(static_cast<int>(p.y))
{}

#endif
