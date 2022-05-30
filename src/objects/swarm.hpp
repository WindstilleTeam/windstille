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

#ifndef HEADER_WINDSTILLE_OBJECTS_SWARM_HPP
#define HEADER_WINDSTILLE_OBJECTS_SWARM_HPP

#include "engine/entity.hpp"

namespace windstille {

class SwarmAgent
{
public:
  glm::vec2 pos;
  glm::vec2 last_pos;
  float  angle;
  float  speed;
  float  max_speed;
  float  turn_speed;

  SwarmAgent()
    : pos(),
      last_pos(),
      angle(),
      speed(),
      max_speed(),
      turn_speed()
  {}
};

/** */
class Swarm : public Entity
{
private:
  typedef std::vector<SwarmAgent> Agents;
  Agents agents;

  glm::vec2 target;

  float turn_speed;

public:
  Swarm(ReaderMapping const& reader);

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;

private:
  Swarm (Swarm const&);
  Swarm& operator= (Swarm const&);
};

} // namespace windstille

#endif

/* EOF */
