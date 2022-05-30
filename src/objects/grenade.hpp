/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_GRENADE_HPP
#define HEADER_WINDSTILLE_OBJECTS_GRENADE_HPP

#include "engine/physics.hpp"
#include "sprite3d/sprite3d.hpp"

namespace windstille {

class Grenade : public Entity
{
public:
  Grenade();
  ~Grenade() override;

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;
  void set_velocity(glm::vec2 const& vel);

private:
  Sprite3D sprite;
  Physics physics;
  std::unique_ptr<CollisionObject> c_object;
};

} // namespace windstille

#endif

/* EOF */
