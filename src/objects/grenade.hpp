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

class Grenade : public Entity
{
public:
  Grenade();
  virtual ~Grenade();

  void draw(SceneContext& sc);
  void update(float delta);
  void set_velocity(const Vector2f& vel);

private:
  Sprite3D sprite;
  Physics physics;
  boost::scoped_ptr<CollisionObject> c_object;
};

#endif

/* EOF */
