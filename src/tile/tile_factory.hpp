/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#include <geom/geom.hpp>
#include <surf/fwd.hpp>

#include <surf/software_surface.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/texture_packer.hpp>
#include "tile/tile_description.hpp"
#include "util/currenton.hpp"
#include "util/pathname.hpp"

namespace windstille {

class Tile;

class TileFactory : public Currenton<TileFactory>
{
private:
  typedef std::vector<Tile*> Tiles;
  Tiles tiles;
  /** The tile images on shared atlas pages */
  wstdisplay::TexturePacker m_packer;

  friend class TileDescription;

  std::vector<TileDescription*> descriptions;

public:
  typedef Tiles::iterator iterator;

  iterator begin() { return tiles.begin(); }
  iterator end()   { return tiles.end(); }

  /** Create a TileFactory from a given tile definition file */
  TileFactory(Pathname const& filename, wstdisplay::Device& device);
  ~TileFactory() override;

  /**
   * Create a new tile, or loads&create it if it is not already
   * available
   */
  Tile* create(int tile_id);

  /**
   * Adds a surface to the TileFactory
   */
  void pack(int id, int colmap, surf::SoftwareSurface const& image, geom::irect const& rect);

private:
  void parse_tiles(ReaderMapping const& reader);
};

} // namespace windstille

#endif

/* EOF */
