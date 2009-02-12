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

#ifndef HEADER_WINDSTILLE_COLLISION_COLLISION_OBJECT_HPP
#define HEADER_WINDSTILLE_COLLISION_COLLISION_OBJECT_HPP

#include <boost/signals.hpp>
#include "math/vector2f.hpp"
#include "math/rect.hpp"
#include "collision_data.hpp"

class DrawingContext;
class TileMap;
class CollisionEngine;
class GameObject;

/** The CollisionObject provides way to handle collisions in a nice
    and predictable way. To use it a GameObject/Entity simply
    registeres a CollisionObject in the CollisionEngine and updates
    its position via set_velocity(). As soon as the CollisionEngine
    registers a collision the CollisionObject::collision() signal is
    called to which the user can register a callback function. In the
    collision() callback he can then handle the collision reaction. */
class CollisionObject
{
public:
  enum ObjectType { RECTANGLE, TILEMAP };
private:
  ObjectType object_type;

  /// position of the object
  Vector2f pos;

  /// velocity of the object
  Vector2f velocity;

  GameObject* game_object;

  boost::signal<void (const CollisionData &)> collision;

  Rectf primitive;
  TileMap* tilemap;

  bool is_unstuckable;
  bool is_unstuck_movable;

  unsigned int is_domains;
  unsigned int check_domains;

public:
  /** Domains provide a way to logically seperate objects from each
      other, so that for example enemies don't check collisions
      against other enemies, but only against the player */
  enum Domains {
    DOMAIN_TILEMAP = (1 << 0),
    DOMAIN_PLAYER  = (1 << 1), 
    DOMAIN_ENEMY   = (1 << 2)
  };

  CollisionObject(GameObject* object, const Rectf& rect_);
  CollisionObject(TileMap* tilemap_);

  virtual ~CollisionObject();

  ObjectType get_type() const { return object_type; }

  /** Sets the velocity of this object, this is the primary way how
      you move a CollisionObject. Velocity simply means the difference
      between the old position and the new position, no acceleration
      or deceleration is involved, so the velocity stays constant for
      the CollisionObject unless you change it with a new call to
      set_velocity */
  void set_velocity(const Vector2f &v);
  Vector2f get_velocity() const;

  /** Sets the CollisionObject to the given pos, it should only be
      called to set the initial position and *not* to update the
      position each update(), see \a set_velocity()
      
      FIXME: unstucking is currently not handled special here, but
      simply in the next run of the collision engine, this should
      probally be changed so that the object is unstuck here without
      affecting other objects and then the real placement position is
      returned */
  void set_pos(const Vector2f& p);
  Vector2f get_pos() const;

  void set_game_object(GameObject* game_object);
  GameObject* get_game_object() const;

  void update(float delta);

  // debugging helpers
  void draw(DrawingContext& dc);

  // this functions support unstucking, which needs to be done, when more than 2 object stack over one another
  // should this object be unstuck ??
  bool unstuck() const { return is_unstuckable; }
  // is this object movable within unstucking ?
  bool unstuck_movable() const { return is_unstuck_movable; }
  
  void set_unstuck(bool s) { is_unstuckable = s; }
  void set_unstuck_movable(bool s) { is_unstuck_movable = s; }

  /** Domains which this object 'is' */
  unsigned int get_is_domains() const;
  void         set_is_domains(unsigned int d);

  /** Domains against which the collision object should check */
  unsigned int get_check_domains() const;
  void         set_check_domains(unsigned int d);

  boost::signal<void (const CollisionData &)>& sig_collision() { return collision; }

  friend class CollisionEngine;
};

#endif

/* EOF */
