/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision.hxx
 * by David Kamphausen (david.kamphausen@web.de)
 *    Ingo Ruhnke
 *
 * The "Windstille" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef HEADER_COLLISION_DATA_HXX
#define HEADER_COLLISION_DATA_HXX

#include <iostream>
#include "math/vector.hpp"

class CollisionObject;

struct CollisionData
{
  enum State { NONE, STUCK, COLLISION } state;

  /**
   * This is a unit vector that specifies the direction of the collision.
   * It's the normal vector of the side/face that we collided with. The vector
   * points away from the object we collided with.
   */
  Vector direction;
  
  // size of time frame
  float delta; 

  // time of collision
  float col_time;

  CollisionObject* object1;
  CollisionObject* object2;

  CollisionData()
  {
    state    = NONE;
    col_time = 0;
  }

  CollisionData merge(const CollisionData &r);

  CollisionData invert() const
  {
    CollisionData r(*this);
    r.direction*=-1;
    r.object1 = object2;
    r.object2 = object1;

    return r;
  } 
};

inline std::ostream &operator<<(std::ostream &o, const CollisionData &r)
{
  o << "(" << r.direction.x << ", " << r.direction.y << ", " << r.col_time << ")";
  return o;
}

#endif

/* EOF */