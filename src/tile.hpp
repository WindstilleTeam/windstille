//  $Id$
// 
//  Windstille - A Jump'n Shoot Game
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

#ifndef HEADER_TILE_HXX
#define HEADER_TILE_HXX

#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "display/texture.hpp"
#include "math/rect.hpp"

enum TileType {
  TILE_EMPTY  = 0,
  TILE_SOLID  = (1 << 0),
  TILE_STAIRS = (1 << 1),
  TILE_LEFT   = (1 << 2),
  TILE_RIGHT  = (1 << 3)
};

class TileDescription;

/** A Tile is a surface or sprite together with information for
 *  collision detection (aka colmap). The collision map is at a
 *  resolution of 8x8 bits. Position information is handled in the
 *  TileMap and not here. (flyweight pattern). */
class Tile
{
public:
  /** The id of the Tile by which the tilemap refers to it */
  int id; 

  // FIXME: Using a surface here might be a worthy consideration, just
  // need to find a way to properly bucket-sort the request

  /**
   * Id of the packer
   */
  int     packer;
  Rectf   uv;
  Texture texture;

  /** bitmap that holds the collision attributes for this tile */
  unsigned int colmap;
  TileDescription* desc;

  Tile(unsigned int arg_colmap);

  unsigned get_colmap() const { return colmap; }
};

#endif

/* EOF */
