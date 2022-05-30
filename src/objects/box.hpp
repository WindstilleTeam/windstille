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

#ifndef HEADER_WINDSTILLE_OBJECTS_BOX_HPP
#define HEADER_WINDSTILLE_OBJECTS_BOX_HPP

#include "engine/entity.hpp"
#include "collision/collision_object.hpp"
#include "engine/sector.hpp"
#include "particles/particle_system.hpp"
#include "sprite2d/sprite.hpp"

class Box : public Entity
{
private:
  Sprite sprite;
  CollisionObject* colobj;
  float gravity;

public:
  Box(ReaderMapping const& props);
  ~Box() override;

  void collision(CollisionData const& data);

  void update(float delta) override;

  void draw(wstdisplay::SceneContext& gc) override;

private:
  Box(Box const&);
  Box& operator=(Box const&);
};

#endif

/* EOF */
