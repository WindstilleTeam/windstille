/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_MATH_MATH_HPP
#define HEADER_WINDSTILLE_MATH_MATH_HPP

#include <assert.h>
#include <limits>
#include <math.h>

namespace math {

const float pi = 3.14159265358979323846f;

template<class T>
T min (const T& a, const T& b)
{
  if (a < b)
    return a;
  else
    return b;
}

template<class T>
T max (const T& a, const T& b)
{
  if (a > b)
    return a;
  else
    return b;
}

template<class T>
T mid (const T& a, const T& b, const T& c)
{
  return max<T>((a), min<T>((b), (c)));
}

inline float normalize_angle(float radians)
{
  radians = fmodf(radians, 2.0f * math::pi);
  if (radians < 0.0f)
    radians += 2.0f * math::pi;
  // Floating point math is so loathsome.  In sp98test, the assertion
  // was barfing at P = 180 because a very small negative number plus
  // 2 PI was equalling 2 PI.  Gakk!
  if (radians == 2.0f * math::pi)
    radians = 0.0f;

  // FIXME: This gets triggered from time to time!
  //  assert (radians >= 0.0 && radians < 2.0 * math::pi);
  return radians;
}

inline bool is_power_of_two(int n)
{
  return (n > 0) && ((n & (n - 1)) == 0);
}

template<typename T>
inline T round_to_power_of_two(T n)
{
  n = n - 1;

  int bits = std::numeric_limits<T>::digits;
  for(int i = 1; i < bits; i *= 2)
    n = n | (n >> i);

  n = n + 1;

  return n;
}

inline float deg2rad(float degree)
{
  return degree / 180.0f * math::pi;
}

inline float rad2deg(float radians)
{
  return radians / math::pi * 180.0f;
}

} // namespace math

#endif

/* EOF */
