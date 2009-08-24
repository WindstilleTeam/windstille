/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include <math.h>
#include "vector3.hpp"
#include "math/matrix.hpp"

class Quaternion
{
public:
  float w;
  float x;
  float y;
  float z;

  Quaternion()
    : w(0), x(0), y(0), z(0)
  {}

  Quaternion(float w_, float x_, float y_, float z_)
    : w(w_), x(x_), y(y_), z(z_)
  {}

  /** Construct a Quaternion representing a rotation 
   *  @param axis   the axis of rotation, must be a unit vector (length = 1.0f)
   *  @param theta  the angle of rotation in radians
   */
  Quaternion(const Vector3& axis, float theta)
    : w(cosf(theta/2)),
      x(),
      y(),
      z()
  {
    float s = sinf(theta/2.0f);
    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
  }

  static Quaternion identity() {
    return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
  }

  float magnitude() const;
  void normalize();

  const Quaternion& operator*= (const Quaternion& o)
  {
    *this = *this * o;
    return *this;
  }

  Quaternion operator* (const Quaternion& o) const
  {
    return Quaternion(
      w*o.w - x*o.x - y*o.y - z*o.z,
      w*o.x + x*o.w + y*o.z - z*o.y,
      w*o.y + y*o.w + z*o.x - x*o.z,
      w*o.z + z*o.w + x*o.y - y*o.x);
  }

  Quaternion operator- (const Quaternion& o) const
  {
    return Quaternion(w-o.w, x-o.x, y-o.y, z-o.z);
  }

  Quaternion operator+ (const Quaternion& o) const
  {
    return Quaternion(w+o.w, x+o.x, y+o.y, z+o.z);
  }

  Quaternion operator* (float s) const
  {
    return Quaternion(w*s, x*s, y*s, z*s);
  }

  float dot_product(const Quaternion& o) const
  {
    return x*o.x + y*o.y + z*o.z + w*o.w;
  }

  Matrix to_matrix() const;
  /**
   * spherical linear interpolation
   * Returns this quaternion rotation added with t* the way from this quaternion
   * to the o quaternion (so t should be between 0 and 1 usually)
   */
  Quaternion slerp(const Quaternion& o, float t) const;
};

#endif

/* EOF */
