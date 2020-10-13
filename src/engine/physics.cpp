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

#include "engine/physics.hpp"

#include <functional>

#include "collision/collision_object.hpp"

Physics::Physics(Entity* entity_) :
  entity(entity_),
  mass(200.0),
  bounciness(1.0),
  force(),
  air_friction(0.0),
  contact_friction(0.0)
{
}

Physics::~Physics()
{
}

void
Physics::register_collobj(CollisionObject& object)
{
  object.sig_collision().connect(std::bind(&Physics::collision, this, std::placeholders::_1));
}

void
Physics::collision(const CollisionData& data)
{
  GameObject* other_object = data.object2->get_game_object();

  const Physics* physics = dynamic_cast<const Physics*> (other_object);
  if(physics)
  {
    elastic_collision(data, *physics);
    data.object1->set_velocity(velocity());
    return;
  }

  // assume fix object...
  bounce_collision(data);
  data.object1->set_velocity(velocity());
}

void
Physics::elastic_collision(const CollisionData& data, const Physics& other)
{
  std::cout << "elastic collision." << std::endl;
  // we could calculate this cheaper if we'd do it once for both objects and not
  // each object on it's own...

  glm::vec2 other_collision_vel
    = data.direction * (other.velocity() * data.direction);
  glm::vec2 collision_vel
    = data.direction * (velocity() * data.direction);

  glm::vec2 new_v = collision_vel * (mass - other.mass);
  new_v += other_collision_vel * (2 * other.mass);
  new_v /= mass + other.mass;

  velocity() += new_v - collision_vel;
  //force += (new_v - collision_vel) * mass / data.delta;

  // TODO apply friction here?
}

void
Physics::bounce_collision(const CollisionData& data)
{
  std::cout << "bounce collision." << std::endl;
  glm::vec2 collision_vel = data.direction * (velocity() * data.direction);

  velocity() -= collision_vel * (1.0f + bounciness);
  //force -= collision_vel * (1.0f + bounciness) * mass / data.delta;

  // TODO apply friction
}

void
Physics::update(float delta)
{
  // add gravity force (TODO make it configurable per Sector)
  force += glm::vec2(0, 9.81f * mass);

  //force -= velocity() * air_friction;

  glm::vec2 acceleration = force / mass;
  velocity() += acceleration * delta;
  pos() += velocity() * delta;

  force = glm::vec2(0, 0);
}

/* EOF */
