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

#include "objects/elevator.hpp"
#include "engine/sector.hpp"
#include "collision/collision_engine.hpp"

Elevator::Elevator(ReaderMapping const& props) :
  size(),
  colobject(),
  sprite()
{
  std::string spritename;

  props.read("sprite", spritename);
  props.read("pos", pos);
  props.read("name", name);

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in Elevator");

  sprite = Sprite(Pathname(spritename));
  size  = Sizef(128, 64);
  colobject = new CollisionObject(this, Rectf(Vector2f(0,0), size));
  Sector::current()->get_collision_engine()->add(colobject);
  colobject->set_pos(pos);
}

Elevator::~Elevator()
{
  if(Sector::current()->get_collision_engine())
    Sector::current()->get_collision_engine()->remove(colobject);
}

void
Elevator::draw(SceneContext& sc)
{
  sc.color().draw(sprite, pos, 10.0f);
}

void
Elevator::update(float delta)
{
  sprite.update(delta);
  pos = colobject->get_pos();
}

/* EOF */
