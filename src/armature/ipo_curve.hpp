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

#ifndef HEADER_WINDSTILLE_ARMATURE_IPO_CURVE_HPP
#define HEADER_WINDSTILLE_ARMATURE_IPO_CURVE_HPP

#include <vector>

#include <glm/glm.hpp>

struct BezierPoint
{
  glm::vec2 handle_left;
  glm::vec2 handle_right;

  glm::vec2 pos;
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

  void add_point(BezierPoint const& p);

private:
  IpoCurve (IpoCurve const&);
  IpoCurve& operator= (IpoCurve const&);
};

#endif

/* EOF */
