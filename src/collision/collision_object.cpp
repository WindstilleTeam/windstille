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

#include "collision/collision_object.hpp"

#include "display/color.hpp"
#include "display/drawing_context.hpp"

/***********************************************************************
 * CollisionObject
 ***********************************************************************/

CollisionObject::CollisionObject(GameObject* game_object_, const Rectf& rect_)
  : object_type(RECTANGLE),
    pos(0,0),
    velocity(0,0),
    game_object(game_object_),
    collision(),
    primitive(rect_),
    tilemap(),
    is_unstuckable(true),
    is_unstuck_movable(true),
    is_domains(DOMAIN_PLAYER  | DOMAIN_ENEMY),
    check_domains(DOMAIN_TILEMAP | DOMAIN_PLAYER | DOMAIN_ENEMY)
{
}

CollisionObject::CollisionObject(TileMap* tilemap_)
  : object_type(),
    pos(),
    velocity(),
    game_object(),
    collision(),
    primitive(),
    tilemap(tilemap_),
    is_unstuckable(),
    is_unstuck_movable(),
    is_domains(),
    check_domains() 
{
  object_type        = TILEMAP;
  is_unstuckable     = true;
  is_unstuck_movable = false;
  velocity           = Vector2f(0,0);
  pos                = Vector2f(0,0);
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
  Vector2f v = get_pos ();
  Rectf  r = primitive;

  r += v;

  dc.fill_rect(r, Color(1.0f, 1.0f, 1.0f), 100.0f);
  
  dc.draw_rect(r, Color(0.6f, 0.6f, 0.6f), 100.0f);
  
  dc.draw_line(Vector2f(r.left + r.get_width ()/2,
                        r.top  + r.get_height ()/2),
               Vector2f(r.left + r.get_width ()/2  + get_velocity ().x,
                        r.top  + r.get_height ()/2 + get_velocity ().y),
               Color(1.0f, 0, 1.0f), 100.0f);
}

void CollisionObject::update(float delta)
{
  pos += velocity * delta;
}

void 
CollisionObject::set_velocity(const Vector2f &m)
{
  velocity=m;
}

Vector2f
CollisionObject::get_pos() const
{
  return pos;
}

Vector2f
CollisionObject::get_velocity() const
{
  return velocity;
}

void
CollisionObject::set_pos(const Vector2f& p)
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
