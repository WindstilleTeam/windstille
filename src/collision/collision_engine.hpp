/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>,
**                     Ingo Ruhnke <grumbel@gmx.de>
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

#include "collision/collision_object.hpp"

class DrawingContext;

class CollisionEngine
{
private:
  typedef std::vector<CollisionObject*> Objects;
  Objects objects;
  float unstuck_velocity;

public:
  CollisionEngine();
  ~CollisionEngine();

  void draw(DrawingContext& dc);
  void update(float delta);
  void update(CollisionObject& obj, float delta);
  void collision(const CollisionData &result);

  CollisionObject* add(CollisionObject *obj);
  void remove(CollisionObject *obj);

  /** Shoots a ray from \a pos into direction \a angle, returns the
      position were the ray collides with the tilemap FIXME: Should be
      extended to handle objects as well */
  Vector2f raycast(const Vector2f& pos, float angle);

private:
  void unstuck(CollisionObject& a, CollisionObject& b, float delta);
  CollisionData collide(CollisionObject& a, CollisionObject& b, float delta);
  CollisionData collide(const Rectf& b1, const Rectf& b2,
                        const Vector2f& b1_v, const Vector2f& b2_v,
                        float delta);
  CollisionData collide_tilemap(CollisionObject& a, CollisionObject& b, float delta);

  void unstuck_tilemap(CollisionObject& a, CollisionObject& b, float delta);
  void unstuck_rect_rect(CollisionObject& a, CollisionObject& b, float delta);
};

#endif

/* EOF */
