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

#ifndef HEADER_WINDSTILLE_ENGINE_PHYSICS_HPP
#define HEADER_WINDSTILLE_ENGINE_PHYSICS_HPP

#include "collision/collision_data.hpp"
#include "entity.hpp"

class Physics
{
public:
  Physics(Entity* entity);
  ~Physics();

  void update(float delta);
  void register_collobj(CollisionObject& object);

  Vector2f& pos() const
  { return entity->pos; }
  Vector2f& velocity() const
  { return entity->velocity; }

private:
  void collision(const CollisionData& data);
  
  void elastic_collision(const CollisionData& data, const Physics& other);
  void bounce_collision(const CollisionData& data);

  Entity* entity;
  float mass;
  float bounciness;
  Vector2f force;
  float air_friction;
  float contact_friction;
};

#endif

/* EOF */
