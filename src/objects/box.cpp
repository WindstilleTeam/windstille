/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
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

#include "objects/box.hpp"

#include <functional>

#include "collision/collision_engine.hpp"

Box::Box(const FileReader& props) :
  sprite(),
  colobj(nullptr),
  gravity()
{
  gravity = 0.0f;
  float width  = 64.0f;
  float height = 64.0f;
  std::string spritename;
  Vector2f vel;

  props.get("sprite", spritename);
  props.get("pos", pos);
  props.get("width", width);
  props.get("height", height);
  props.get("vel", vel);
  props.get("name", name);
  props.get("gravity", gravity);

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in Box");

  sprite = Sprite(Pathname(spritename));

  colobj = new CollisionObject(this, Rectf(0, 0, width, height));
  colobj->set_velocity(vel);
  colobj->set_pos(Vector2f(pos.x, pos.y));

  Sector::current()->get_collision_engine()->add(colobj);

  colobj->sig_collision().connect(std::bind(&Box::collision, this, std::placeholders::_1));
}

Box::~Box()
{
  Sector::current()->get_collision_engine()->remove(colobj);
}

void
Box::collision(const CollisionData& data)
{
  //std::cout << this << ": Collision Event" << std::endl;
  if ((data.direction.x > 0 && colobj->get_velocity().x < 0) ||
      (data.direction.x < 0 && colobj->get_velocity().x > 0))
  {
    colobj->set_velocity(Vector2f(-colobj->get_velocity().x, colobj->get_velocity().y));
  }

  if ((data.direction.y > 0 && colobj->get_velocity().y < 0) ||
      (data.direction.y < 0 && colobj->get_velocity().y > 0))
  {
    colobj->set_velocity(Vector2f(colobj->get_velocity().x, -colobj->get_velocity().y));
  }
}

void
Box::update(float delta)
{
  colobj->set_velocity(Vector2f(colobj->get_velocity().x,
                                colobj->get_velocity().y + gravity * delta));

  sprite.update(delta);
  pos = colobj->get_pos();
}

void
Box::draw(SceneContext& sc)
{
  sc.color().draw(sprite, pos, 10.0f);
}

/* EOF */
