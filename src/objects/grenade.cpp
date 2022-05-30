/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "app/app.hpp"
#include "collision/collision_engine.hpp"
#include "engine/sector.hpp"
#include "objects/grenade.hpp"
#include "sprite2d/manager.hpp"
#include "sprite3d/manager.hpp"

Grenade::Grenade() :
  sprite(g_app.sprite3d().create(Pathname("models/objects/grenade/grenade.wsprite"))),
  physics(this),
  c_object(new CollisionObject(this, geom::frect(0, 0, 32, 32)))
{
  physics.register_collobj(*c_object);

  Sector::current()->get_collision_engine()->add(c_object.get());
}

Grenade::~Grenade()
{
}

void
Grenade::draw(wstdisplay::SceneContext& sc)
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
Grenade::set_velocity(glm::vec2 const& velocity_)
{
  velocity = velocity_;
}

/* EOF */
