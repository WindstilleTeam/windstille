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

#ifndef __VECTOR3_HPP__
#define __VECTOR3_HPP__

#include <iostream>
#include <glm/glm.hpp>

typedef glm::vec3 Vector3;

#include "math/matrix.hpp"

#if 0
/**
 * A 3-dimensional vector
 */
class Vector3
{
public:
  float x;
  float y;
  float z;

  Vector3()
    : x(0), y(0), z(0)
  {}

  Vector3(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_)
  {}

  bool operator== (const Vector3& o) const
  {
    return x == o.x && y == o.y && z == o.z;
  }

  bool operator !=(const Vector3& o) const
  {
    return x != o.x || y != o.y || z != o.z;
  }

  Vector3 operator- () const
  {
    return Vector3(-x, -y, -z);
  }

  Vector3 operator+ (const Vector3& o) const
  {
    return Vector3( x+o.x, y+o.y, z+o.z );
  }

  Vector3 operator- (const Vector3& o) const
  {
    return Vector3( x-o.x, y-o.y, z-o.z );
  }

  Vector3 operator* (float s) const
  {
    return Vector3(x * s, y * s, z * s);
  }

  const Vector3& operator+= (const Vector3& o)
  {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
  }

  const Vector3& operator-= (const Vector3& o)
  {
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
  }

  const Vector3& operator*= (const Vector3& o)
  {
    x *= o.x;
    y *= o.y;
    z *= o.z;
    return *this;
  }

  Matrix to_matrix() const
  {
    Matrix result = Matrix::identity();
    result.matrix[12] = x;
    result.matrix[13] = y;
    result.matrix[14] = z;
    return result;
  }
};

inline std::ostream& operator<<(std::ostream& s, const Vector3& v) {
  return (s << "[" << v.x << ", " << v.y << ", " << v.z << "]");
}
#endif

#endif
