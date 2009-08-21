/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_TILE_TILE_FACTORY_HPP
#define HEADER_WINDSTILLE_TILE_TILE_FACTORY_HPP

#include <map>
#include <string>
#include "tile_description.hpp"

class Tile;
class TilePacker;

/** */
class TileFactory
{
private:
  typedef std::vector<Tile*> Tiles;
  Tiles tiles;
  typedef std::vector<TilePacker*> TilePackers;
  TilePackers packers;
  int color_packer;

  static TileFactory* current_;

  friend class TileDescription;

  std::vector<TileDescription*> descriptions;
  
public:
  typedef Tiles::iterator iterator;
  
  iterator begin() { return tiles.begin(); }
  iterator end()   { return tiles.end(); }

  /** Create a TileFactory from a given tile definition file */
  TileFactory(const std::string& filename);
  ~TileFactory();
  
  /**
   * Create a new tile, or loads&create it if it is not already
   * available
   */
  Tile* create(int tile_id);

  /** 
   * Adds a surface to the TileFactory
   */
  void pack(int id, int colmap, const SoftwareSurface& image, const Rect& rect);

  /** Access the default TileFactor*/
  static TileFactory* current() { return current_; }

private:
  void parse_tiles(FileReader& reader);
};

#endif

/* EOF */
