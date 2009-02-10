#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include "collision/collision_data.hpp"
#include "entity.hpp"

class Physics
{
public:
  Physics(Entity* entity);
  ~Physics();

  void update(float delta);
  void register_collobj(CollisionObject& object);

  Vector& pos() const
  { return entity->pos; }
  Vector& velocity() const
  { return entity->velocity; }

private:
  void collision(const CollisionData& data);
  
  void elastic_collision(const CollisionData& data, const Physics& other);
  void bounce_collision(const CollisionData& data);

  Entity* entity;
  float mass;
  float bounciness;
  Vector force;
  float air_friction;
  float contact_friction;
};

#endif

