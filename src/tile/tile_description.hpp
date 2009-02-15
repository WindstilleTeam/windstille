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

#ifndef HEADER_WINDSTILLE_TILE_TILE_DESCRIPTION_HPP
#define HEADER_WINDSTILLE_TILE_TILE_DESCRIPTION_HPP

#include <vector>
#include <string>
#include "util/file_reader.hpp"

class TileFactory;

/**
 * The TileDescription descriptes a set of tiles
 */
class TileDescription
{
public:
  std::vector<int> ids;
  std::vector<int> colmap;
  std::string filename;
  int width;
  int height;

  TileDescription(FileReader& props);

  /**
   * Load the TileDescription into the give TileFactory
   */
  void load(TileFactory* factory);
};

#endif

/* EOF */
