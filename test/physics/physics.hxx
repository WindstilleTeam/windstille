//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_PHYSICS_HXX
#define HEADER_PHYSICS_HXX

#include <vector>

/** A rectangular object that can handle physics and collision */
class PhysicObject
{
public: 
  /** Uniq Id which identifies this object */
  int id;

  bool movable;

  // Top/Right corner of the object
  float x_pos;
  float y_pos;

  float mass;

  float width;
  float height;

  float x_velocity;
  float y_velocity;

  /** The velocity which is used to unstuck, stuck objects */
  bool collision;

  PhysicObject(float x_, float y_, float width_, float height_) 
  {
    id = -1;
    movable = true;
    mass = 1.0f;
    collision = false;
    
    x_pos = x_;
    y_pos = y_;
    
    width  = width_;
    height = height_;

    x_velocity = 0.0f;
    y_velocity = 0.0f;
  }
};

inline
std::ostream& operator<<(std::ostream& out, const PhysicObject& obj)
{
  out << obj.id << ":[(" << obj.x_pos << ", "
      << obj.y_pos << "), "
      << "(" << obj.width << ", " << obj.height << ")]+[" << obj.x_velocity << ", " << obj.y_velocity << "]";
  return out;
}

/** */
class Physics
{
private:
  typedef std::vector<PhysicObject> Objects;
  Objects objects;

  float friction;
  float unstuck_velocity;

  float x_acceleration;
  float y_acceleration;

  float minimum_velocity;

  enum CollisionState { COL_AT, COL_ALWAYS, COL_NEVER };

  struct CollisionResult 
  {
    CollisionState state;

    // Time at which the collision starts, only valid if state == COL_AT
    float u0;
    
    // Time at which the collisions ends, only valid if state == COL_AT
    float u1;
  };

  /** Calculate when two objects, given by there position (a,b), there
      width (aw,bw) and there velocity (av,bw) start colliding (u0)
      and end colliding (u1). true is set when u0 and u1 could be
      calculated, false when the relative velocity was zero.
  */
  CollisionResult simplesweep1d(float a, float aw, float av,
                                float b, float bw, float bv);

  void unstuck(PhysicObject& a, PhysicObject& b, float delta);
  void resolve_collision(PhysicObject& a, PhysicObject& b, CollisionResult& x, CollisionResult& y, 
                         float delta);
public:
  enum Side { LEFT, RIGHT, TOP, BOTTOM, STUCK }; 

  Physics();
  ~Physics();

  void draw();
  void update(float delta);
  void update(PhysicObject& obj, float delta);
  void collision(PhysicObject& a, PhysicObject& b, Side side);

  PhysicObject& add_object(PhysicObject obj);
};

#endif

/* EOF */
