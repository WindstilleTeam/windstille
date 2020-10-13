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

#ifndef HEADER_WINDSTILLE_SCREEN_GEOMETRY_TEST_HPP
#define HEADER_WINDSTILLE_SCREEN_GEOMETRY_TEST_HPP

#include "screen/screen.hpp"
#include <glm/glm.hpp>
#include "math/line.hpp"

/** */
class GeometryTest : public Screen
{
private:
  Line line1;
  Line line2;

  glm::vec2 cursor;
  glm::vec2 cursor2;
  glm::vec2 collision_point;

  int point_count;

  bool had_prev_collision;

public:
  GeometryTest();

  void draw() override;
  void update(float delta, const Controller& controller) override;

private:
  GeometryTest (const GeometryTest&);
  GeometryTest& operator= (const GeometryTest&);
};

#endif

/* EOF */
