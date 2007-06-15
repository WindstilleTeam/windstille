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

#include "line.hpp"

Line::Line(const Vector& p1_,
           const Vector& p2_)
  : p1(p1_), p2(p2_)
{
}

float
Line::length() const
{
  return (p2 - p1).length();
}

bool
Line::intersect(const Line& line, float& ua, float& ub)
{
  const float& x1 = p1.x;
  const float& y1 = p1.y;

  const float& x2 = p2.x;
  const float& y2 = p2.y;
  
  const float& x3 = line.p1.x;
  const float& y3 = line.p1.y;

  const float& x4 = line.p2.x;
  const float& y4 = line.p2.y;

  float quotient = ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

  ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / quotient;
  ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / quotient;

  return (ua >= 0.0f && ua <= 1.0f &&
          ub >= 0.0f && ub <= 1.0f);
}

bool
Line::intersect(const Line& line, Vector& colpos)
{
  float ua, ub;
  bool do_collide = intersect(line, ua, ub);

  colpos = p1 + ((p2 - p1) * ua);

  return do_collide;
}

/* EOF */
