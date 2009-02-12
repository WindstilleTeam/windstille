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
#include "math/vector.hpp"

void
Vector::normalize()
{
  float mag = magnitude();
  x /= mag;
  y /= mag;
}

Vector Vector::unit() const
{
  return *this / magnitude();
}

float
Vector::magnitude() const
{
  return sqrt(x*x + y*y);
}

Vector
Vector::rotate(float angle) const
{
  float len = magnitude();
  return Vector(len * cos(angle), len * sin(angle));
}

Vector
Vector::project(const Vector& b)
{
  float dp = this->dot(b);
  return Vector((dp / (b.x*b.x + b.y*b.y) ) * b.x,
                (dp / (b.x*b.x + b.y*b.y) ) * b.y);
}

float
Vector::dot(const Vector& b)
{
  return (x * b.x + y * b.y);
}

bool
Vector::is_null() const
{
  return (x == 0.0f && y == 0.0f);
}

std::ostream& operator<<(std::ostream& s, const Vector& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

Vector operator*(float s, const Vector& v)
{
  return Vector(v.x * s, v.y * s);
}

/* EOF */
