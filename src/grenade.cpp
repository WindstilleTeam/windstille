/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/
#include <config.h>

#include "grenade.hpp"
#include "sector.hpp"
#include "collision/collision_engine.hpp"

Grenade::Grenade()
  : sprite("models/objects/grenade/grenade.wsprite"), physics(this)
{
  c_object.reset(new CollisionObject(this, Rectf(0, 0, 32, 32)));
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
Grenade::set_velocity(const Vector& velocity)
{
  this->velocity = velocity;
}
