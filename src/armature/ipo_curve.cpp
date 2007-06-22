/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include "ipo_curve.hpp"

IpoCurve::IpoCurve()
{
}

IpoCurve::~IpoCurve()
{
}

float
IpoCurve::evalutate(float x)
{
  // find bezier points left and right from t
  Vector left;
  Vector right;

  // calculate the value t from x
  float t;

  // calculate the result

  //def bezier(p0, p1, p2, p3, t):
  //    return p0*(1-t)**3 + 3*p1*t*((1-t)**2) + 3*p2*(t**2)*(1-t) + p3*t**3
}

void
IpoCurve::add_point(const BezierPoint& p)
{
  assert(points.empty() ||
         points.back().point.x < p.x &&
         p.handle_left.x < p.x &&
         p.x < p.handle_right.x);
  points.push_back(p);
}

/* EOF */
