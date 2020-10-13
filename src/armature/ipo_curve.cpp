/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "armature/ipo_curve.hpp"

IpoCurve::IpoCurve() :
  points()
{
}

IpoCurve::~IpoCurve()
{
}

float
IpoCurve::evalutate(float /*x*/)
{
  // find bezier points left and right from t
  //glm::vec2 left;
  //glm::vec2 right;

  // calculate the value t from x
  //float t;

  // calculate the result

  //def bezier(p0, p1, p2, p3, t):
  //    return p0*(1-t)**3 + 3*p1*t*((1-t)**2) + 3*p2*(t**2)*(1-t) + p3*t**3
  return 0.0f;
}

void
IpoCurve::add_point(const BezierPoint& /*p*/)
{
  /*
    assert(points.empty() ||
    points.back().point.x < p.x &&
    p.handle_left.x < p.x &&
    p.x < p.handle_right.x);
    points.push_back(p);
  */
}

/* EOF */
