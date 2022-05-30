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

#include "scripting/game_objects.hpp"

#include "engine/sector.hpp"
#include "engine/game_object.hpp"

namespace Scripting {

std::string const&
GameObject::get_name() const
{
  return object.lock()->get_name();
}

void
GameObject::remove()
{
  object.lock()->remove();
}

void
GameObject::set_active(bool active)
{
  object.lock()->set_active(active);
}

bool
GameObject::is_active() const
{
  return object.lock()->is_active();
}

void
GameObject::set_parent(std::string const& name)
{
  if (windstille::Entity* entity = dynamic_cast<windstille::Entity*>(object.lock().get()))
  {
    if (windstille::GameObject* obj = windstille::Sector::current()->get_object(name))
    {
      entity->set_parent(dynamic_cast<windstille::Entity*>(obj));
    }
  }
}

void
TestObject::set_sprite(std::string const& filename)
{
  obj()->set_sprite(filename);
}

void
TestObject::set_action(std::string const& animation)
{
  obj()->set_action(animation);
}

void
TestObject::set_pos(float x, float y)
{
  obj()->set_pos(glm::vec2(x, y));
}

void
TestObject::set_vflip(bool vflip)
{
  obj()->set_vflip(vflip);
}

void
TestObject::attach(std::string const& spritename,
                   std::string const& attachement_point)
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

