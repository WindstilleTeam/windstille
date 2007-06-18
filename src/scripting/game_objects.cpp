#include "wrapper.interface.hpp"
#include "entity.hpp"
#include "sector.hpp"
#include "game_objects.hpp"

namespace Scripting
{

const std::string&
GameObject::get_name() const
{
  return object->get_name();
}

void
GameObject::remove()
{
  object->remove();
}

void
GameObject::set_active(bool active)
{
  object->set_active(active);
}

void
GameObject::set_parent(const std::string& name)
{
  if (::Entity* entity = dynamic_cast<Entity*>(object.get()))
    {
      if (::GameObject* obj = Sector::current()->get_object(name))
        {
          entity->set_parent(dynamic_cast<Entity*>(obj));
        }
    }
}

void
TestObject::set_sprite(const std::string& filename)
{
  obj()->set_sprite(filename);
}

void
TestObject::set_action(const std::string& animation)
{
  obj()->set_action(animation);
}

void
TestObject::set_pos(float x, float y)
{
  obj()->set_pos(Vector(x, y));
}

void
TestObject::set_vflip(bool vflip)
{
  obj()->set_vflip(vflip);
}

void
TestObject::attach(const std::string& spritename,
		           const std::string& attachement_point)
{
  obj()->attach(spritename, attachement_point);
}

void
Player::start_listening()
{
  obj()->start_listening();
}

void
Player::stop_listening()
{
  obj()->stop_listening();
}

void
ScriptableObject::move_to(float x, float y, float target_speed, float acceleration)
{
  obj()->move_to(x, y, target_speed, acceleration);
}

void
ScriptableObject::start_flash(float speed)
{
  obj()->start_flash(speed);
}

}

