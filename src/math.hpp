/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_MATH_HXX
#define HEADER_MATH_HXX

#include <assert.h>
#include <math.h>

namespace math {

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
  radians = fmod (radians, static_cast<float>(2.0 * M_PI));
  if (radians < 0.0)
    radians += 2.0 * M_PI;
  // Floating point math is so loathsome.  In sp98test, the assertion
  // was barfing at P = 180 because a very small negative number plus
  // 2 PI was equalling 2 PI.  Gakk!
  if (radians == 2.0 * M_PI)
    radians = 0.0;
  
  // FIXME: This gets triggered from time to time!
  //  assert (radians >= 0.0 && radians < 2.0 * M_PI);
  return radians;
}

inline float deg2rad(float degree)
{
  return degree / 180.0f * M_PI;
}

inline float rad2deg(float radians)
{
  return radians / M_PI * 180.0f;
}

} // namespace math

#endif

/* EOF */
