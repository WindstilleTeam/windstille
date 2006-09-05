/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_object.cxx
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

#include <assert.h>

#include "display/drawing_context.hpp"
#include "display/display.hpp"
#include "collision_object.hpp"
#include "collision_engine.hpp"

/***********************************************************************
 * CollisionObject
 ***********************************************************************/

CollisionObject::CollisionObject(GameObject* game_object, const Rectf& rect_)
  : primitive(rect_)
{
  object_type        = RECTANGLE;
  is_unstuckable     = true;
  is_unstuck_movable = true;
  velocity           = Vector(0,0);
  pos                = Vector(0,0);
  game_object        = game_object;

  is_domains    = DOMAIN_PLAYER  | DOMAIN_ENEMY;
  check_domains = DOMAIN_TILEMAP | DOMAIN_PLAYER | DOMAIN_ENEMY;
}

CollisionObject::CollisionObject(TileMap* tilemap_)
  : tilemap(tilemap_)
{
  object_type        = TILEMAP;
  is_unstuckable     = true;
  is_unstuck_movable = false;
  velocity           = Vector(0,0);
  pos                = Vector(0,0);
  game_object        = 0;

  is_domains    = DOMAIN_TILEMAP;
  check_domains = DOMAIN_PLAYER | DOMAIN_ENEMY;
}

CollisionObject::~CollisionObject()
{
}

void
CollisionObject::draw(DrawingContext& dc)
{
  Vector v = get_pos ();
  Rectf  r = primitive;

  r += v;

  dc.fill_rect(r, Color(1.0f, 1.0f, 1.0f), 100.0f);
  
  dc.draw_rect(r, Color(0.6f, 0.6f, 0.6f), 100.0f);
  
  dc.draw_line(Vector(r.left + r.get_width ()/2,
                      r.top  + r.get_height ()/2),
               Vector(r.left + r.get_width ()/2  + get_velocity ().x,
                      r.top  + r.get_height ()/2 + get_velocity ().y),
               Color(1.0f, 0, 1.0f), 100.0f);
}

void CollisionObject::update(float delta)
{
  pos += velocity * delta;
}

void 
CollisionObject::set_velocity(const Vector &m)
{
  velocity=m;
}

Vector
CollisionObject::get_pos() const
{
  return pos;
}

Vector
CollisionObject::get_velocity() const
{
  return velocity;
}

void
CollisionObject::set_pos(const Vector& p)
{
  // FIXME: Do this somewhat more clever to avoid stuck issues
  pos = p;
}

void
CollisionObject::set_game_object(GameObject* object)
{
  game_object = object;
}

GameObject*
CollisionObject::get_game_object() const
{
  return game_object;
}

unsigned int
CollisionObject::get_is_domains() const
{
  return is_domains;
}

void
CollisionObject::set_is_domains(unsigned int d)
{
  is_domains = d;
}

unsigned int
CollisionObject::get_check_domains() const
{
  return check_domains;
}

void
CollisionObject::set_check_domains(unsigned int d)
{
  check_domains = d;
}

/* EOF */
