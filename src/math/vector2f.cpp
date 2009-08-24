//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Matthias Braun <matze@braunis.de
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <cmath>
#include <iostream>

#include "math/vector2f.hpp"

void
Vector2f::normalize()
{
  float mag = magnitude();
  x /= mag;
  y /= mag;
}

Vector2f Vector2f::unit() const
{
  return *this / magnitude();
}

float
Vector2f::magnitude() const
{
  return sqrtf(x*x + y*y);
}

Vector2f
Vector2f::rotate(float angle) const
{
  return Vector2f(x * cosf(angle) - y * sinf(angle),
                  x * sinf(angle) + y * cosf(angle));
}

Vector2f
Vector2f::project(const Vector2f& b)
{
  float dp = this->dot(b);
  return Vector2f((dp / (b.x*b.x + b.y*b.y) ) * b.x,
                (dp / (b.x*b.x + b.y*b.y) ) * b.y);
}

float
Vector2f::dot(const Vector2f& b)
{
  return (x * b.x + y * b.y);
}

bool
Vector2f::is_null() const
{
  return (x == 0.0f && y == 0.0f);
}

std::ostream& operator<<(std::ostream& s, const Vector2f& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

Vector2f operator*(float s, const Vector2f& v)
{
  return Vector2f(v.x * s, v.y * s);
}

/* EOF */
