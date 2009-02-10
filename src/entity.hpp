/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_ENTITY_HXX
#define HEADER_ENTITY_HXX

#include <boost/signal.hpp>
#include "math/vector.hpp"
#include "game_object.hpp"
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
  
  Vector pos;
  Vector velocity;
  bool useable;
  std::string use_verb;
  
  Entity* parent;
public:
  Entity();
  virtual ~Entity();
   
  virtual Vector get_pos() const
  { return pos; }

  virtual void set_pos(Vector pos);

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
