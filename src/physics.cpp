
#include "physics.hpp"

Physics::Physics(Entity* entity)
  : entity(entity)
{
  mass = 200.0;
  bounciness = 1.0;
  air_friction = 0.0;
  contact_friction = 0.0;
}

Physics::~Physics()
{
}

void
Physics::register_collobj(CollisionObject& object)
{
  slots.push_back(object.sig_collision().connect(this, &Physics::collision));
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
  printf("elastic collision.\n");
  // we could calculate this cheaper if we'd do it once for both objects and not
  // each object on it's own...

  Vector other_collision_vel 
    = data.direction * (other.velocity() * data.direction);
  Vector collision_vel
    = data.direction * (velocity() * data.direction);
  
  Vector new_v = collision_vel * (mass - other.mass);
  new_v += other_collision_vel * (2 * other.mass);
  new_v /= mass + other.mass;

  velocity() += new_v - collision_vel;  
  //force += (new_v - collision_vel) * mass / data.delta;

  // TODO apply friction here?
}

void
Physics::bounce_collision(const CollisionData& data)
{
  printf("bounce collision.\n");
  Vector collision_vel = data.direction * (velocity() * data.direction);

  velocity() -= collision_vel * (1.0 + bounciness);
  //force -= collision_vel * (1.0f + bounciness) * mass / data.delta;

  // TODO apply friction
}

void
Physics::update(float delta)
{
  // add gravity force (TODO make it configurable per Sector)
  force += Vector(0, 9.81 * mass);

  //force -= velocity() * air_friction;
  
  Vector acceleration = force / mass;
  velocity() += acceleration * delta;
  pos() += velocity() * delta;

  force = Vector(0, 0);
}

