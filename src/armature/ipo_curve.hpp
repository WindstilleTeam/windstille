/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_ARMATURE_IPO_CURVE_HPP
#define HEADER_WINDSTILLE_ARMATURE_IPO_CURVE_HPP

#include <vector>

#include "math/vector2f.hpp"

struct BezierPoint
{
  Vector2f handle_left;
  Vector2f handle_right;

  Vector2f pos;
};

/** */
class IpoCurve
{
private:
  std::vector<BezierPoint> points;
public:
  IpoCurve();
  ~IpoCurve();
  
  float evalutate(float t);

  void add_point(const BezierPoint& p);

private:
  IpoCurve (const IpoCurve&);
  IpoCurve& operator= (const IpoCurve&);
};

#endif

/* EOF */
