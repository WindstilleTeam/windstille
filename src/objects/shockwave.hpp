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

#ifndef HEADER_WINDSTILLE_OBJECTS_SHOCKWAVE_HPP
#define HEADER_WINDSTILLE_OBJECTS_SHOCKWAVE_HPP

#include <glm/glm.hpp>

#include "engine/game_object.hpp"

namespace windstille {

class SceneContext;

/** A distortion ring spreading from pos. Drawing it needs a copy of
    the screen behind it, which was never finished, so it draws
    nothing. */
class Shockwave : public GameObject
{
private:
  glm::vec2        pos;
  float radius;

public:
  Shockwave(ReaderMapping const& props);
  ~Shockwave() override;

  void draw (SceneContext& context) override;
  void update (float delta) override;

private:
  Shockwave (Shockwave const&);
  Shockwave& operator= (Shockwave const&);
};

} // namespace windstille

#endif

/* EOF */
