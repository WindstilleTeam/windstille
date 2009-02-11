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

#include <assert.h>
#include "globals.hpp"
#include "entity.hpp"
#include "sector.hpp"
#include "tile/tile_map.hpp"

Entity::Entity()
  : velocity(0, 0),
    useable(false)
{
  parent = 0;
}

Entity::~Entity()
{
}

void
Entity::set_pos(Vector pos)
{
  this->pos = pos;
}

bool
Entity::on_ground() const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y+16);
}

bool 
Entity::in_wall() const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y);
}

void
Entity::set_parent(Entity* parent_)
{
  if (parent_ != this)
    {
      parent = parent_;
    }
}

/* EOF */
