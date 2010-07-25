/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "engine/sector.hpp"
#include "collision/collision_engine.hpp"
#include "objects/grenade.hpp"

Grenade::Grenade() :
  sprite(Pathname("models/objects/grenade/grenade.wsprite")), 
  physics(this),
  c_object(new CollisionObject(this, Rectf(0, 0, 32, 32)))
{
  physics.register_collobj(*c_object);

  Sector::current()->get_collision_engine()->add(c_object.get());
}

Grenade::~Grenade()
{
}

void
Grenade::draw(SceneContext& sc)
{
  sprite.draw(sc.color(), pos, 0);
}

void
Grenade::update(float delta)
{
  sprite.update(delta);
  physics.update(delta);
  c_object->set_pos(pos);
  c_object->set_velocity(velocity);
}

void
Grenade::set_velocity(const Vector2f& velocity_)
{
  velocity = velocity_;
}

/* EOF */
