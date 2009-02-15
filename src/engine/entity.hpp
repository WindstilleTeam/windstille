/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_ENGINE_ENTITY_HPP
#define HEADER_WINDSTILLE_ENGINE_ENTITY_HPP

#include <boost/signal.hpp>
#include "math/vector2f.hpp"
#include "engine/game_object.hpp"
#include "collision/collision_object.hpp"

/** A GameObject which has a position and some other properties which
    are shared among all/most things in the world */
class Entity : public GameObject
{
private:
  boost::signal<void ()> done; 

protected:
  friend class Physics;
  bool on_ground() const;
  bool in_wall() const;
  
  Vector2f pos;
  Vector2f velocity;
  bool useable;
  std::string use_verb;
  
  Entity* parent;

public:
  Entity();
  virtual ~Entity();
   
  virtual Vector2f get_pos() const
  { return pos; }

  virtual void set_pos(Vector2f pos);

  /** Called once per frame to allow the object to draw itself */
  virtual void draw(SceneContext& gc) = 0;

  /** Called, possible multiple times, per frame to let the object
      update its position and properties */
  virtual void update(float delta) = 0;

  /** Called when the player uses this entity */
  virtual void use() {}

  /** Returns true if the entity is useable by the player */
  virtual bool is_useable() const { return useable; }
  std::string get_use_verb() const { return use_verb; }

  /** Makes the objects (un)usable for the player */
  void set_useable(bool t) { useable = t; }

  void set_parent(Entity* parent_);

  boost::signal<void ()>& sig_done() { return done; }

private:
  Entity (const Entity&);
  Entity& operator= (const Entity&);
};

#endif

/* EOF */
