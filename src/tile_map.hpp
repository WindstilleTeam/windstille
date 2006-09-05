//  $Id$
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_TILEMAP_HXX
#define HEADER_WINDSTILLE_TILEMAP_HXX

#include <string>

#include "globals.hpp"
#include "field.hpp"
#include "lisp/lisp.hpp"
#include "game_object.hpp"
#include "display/scene_context.hpp"

class Tile;

class TileMap : public GameObject
{
private:
  Field<Tile*> field;
  typedef Field<Tile*>::iterator FieldIter;
  float z_pos; 
  float total_time;

public:
  TileMap(FileReader& props);
  virtual ~TileMap();

  void update (float delta);
  void draw (SceneContext& gc);
  
  /** @return the type of ground at the given world coordinates */
  bool is_ground(float x, float y);

  /** @return the type of ground at the given subtile coordinates */
  unsigned int get_pixel(int x, int y);
  
  int get_width () const { return field.get_width(); }
  int get_height () const { return field.get_height (); }

  int get_tile_size () const { return TILE_SIZE; }

  /** Shoots a ray from \a pos into direction \a angle, returns the
      position were the ray collides with the tilemap */
  Vector raycast(const Vector& pos, float angle);
};

#endif

/* EOF */
