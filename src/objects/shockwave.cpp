/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "objects/shockwave.hpp"

namespace windstille {

Shockwave::Shockwave(ReaderMapping const& props) :
  pos(),
  radius()
{
  props.read("pos", pos);
  radius = 100.0f;
}

Shockwave::~Shockwave()
{
}

void
Shockwave::draw (SceneContext& /*sc*/)
{
}

void
Shockwave::update (float delta)
{
  radius += 150.0f * delta;
  if (radius > 300.0f)
    radius = 0;
}

} // namespace windstille

/* EOF */
