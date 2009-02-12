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

#include "wrapper.interface.hpp"
#include "engine/entity.hpp"
#include "engine/sector.hpp"
#include "game_objects.hpp"

namespace Scripting {

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
  obj()->set_pos(Vector2f(x, y));
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

} // namespace Scripting

/* EOF */


