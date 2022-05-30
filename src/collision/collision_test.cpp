/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>,
**                     Ingo Ruhnke <grumbel@gmail.com>
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

#include "collision/collision_test.hpp"
#include <assert.h>

namespace windstille {

SweepResult simple_sweep_1d(float a, float aw, float av,
                            float b, float bw, float bv)
{
  SweepResult res;
  // Normalize the calculation so that only A moves and B stands still
  float v = av - bv;

  if (v > 0)
  {
    res.t0 = (b - (a + aw)) / v;
    res.t1 = (b + bw - a) / v;
    res.state = SweepResult::COL_AT;

    assert(res.t0 <= res.t1);
  }
  else if (v < 0)
  {
    res.t0 = (b + bw - a) / v;
    res.t1 = (b - (a + aw)) / v;
    res.state = SweepResult::COL_AT;

    assert(res.t0 <= res.t1);
  }
  else // (v == 0)
  {
    if ((a + aw) < b || (a > b + bw))
      res.state = SweepResult::COL_NEVER;
    else
      res.state = SweepResult::COL_ALWAYS;
  }
  return res;
}

} // namespace windstille

/* EOF */
