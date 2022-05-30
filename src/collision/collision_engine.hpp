/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>,
**                     Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_COLLISION_COLLISION_ENGINE_HPP
#define HEADER_WINDSTILLE_COLLISION_COLLISION_ENGINE_HPP

#include <vector>

#include <wstdisplay/fwd.hpp>

#include "collision/collision_object.hpp"

namespace windstille {

class CollisionEngine
{
private:
  typedef std::vector<CollisionObject*> Objects;
  Objects objects;
  float unstuck_velocity;

public:
  CollisionEngine();
  ~CollisionEngine();

  void draw(wstdisplay::DrawingContext& dc);
  void update(float delta);
  void update(CollisionObject& obj, float delta);
  void collision(const CollisionData &result);

  CollisionObject* add(CollisionObject *obj);
  void remove(CollisionObject *obj);

  /** Shoots a ray from \a pos into direction \a angle, returns the
      position were the ray collides with the tilemap FIXME: Should be
      extended to handle objects as well */
  glm::vec2 raycast(glm::vec2 const& pos, float angle);

private:
  void unstuck(CollisionObject& a, CollisionObject& b, float delta);
  CollisionData collide(CollisionObject& a, CollisionObject& b, float delta);
  CollisionData collide(geom::frect const& b1, geom::frect const& b2,
                        glm::vec2 const& b1_v, glm::vec2 const& b2_v,
                        float delta);
  CollisionData collide_tilemap(CollisionObject& a, CollisionObject& b, float delta);

  void unstuck_tilemap(CollisionObject& a, CollisionObject& b, float delta);
  void unstuck_rect_rect(CollisionObject& a, CollisionObject& b, float delta);
};

} // namespace windstille

#endif

/* EOF */
