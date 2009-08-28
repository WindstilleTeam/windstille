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

#include "engine/entity.hpp"

#include "properties/property_set.hpp"
#include "engine/sector.hpp"
#include "tile/tile_map.hpp"

Entity::Entity()
  : done(),
    pos(),
    velocity(0, 0),
    useable(false),
    use_verb(),
    parent(0)
{
}

Entity::~Entity()
{
}

void
Entity::set_pos(Vector2f pos_)
{
  pos = pos_;
}

bool
Entity::on_ground() const
{
  return Sector::current()->get_tilemap()->is_ground(pos.x, pos.y+16);
}

bool 
Entity::in_wall() const
{
  return Sector::current()->get_tilemap()->is_ground(pos.x, pos.y);
}

void
Entity::set_parent(Entity* parent_)
{
  if (parent_ != this)
    {
      parent = parent_;
    }
}

Properties
Entity::get_properties()
{
  static PropertySet* type = 0;
  if (!type) 
    {
      type = new PropertySet("Entity");
      type->add_bool("usable", &Entity::useable);
    }

  return Properties(type, this);
}

/* EOF */
