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

#include "tile/tile_factory.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "tile/tile.hpp"
#include "tile/tile_packer.hpp"
#include <wstdisplay/software_surface.hpp>

namespace {

/** Check if the given region of the given image is fully transparent */
bool surface_empty(SoftwareSurface const& image, int sx, int sy, int w, int h)
{
  uint8_t const* data = static_cast<uint8_t const*>(image.get_data());

  for(int y = sy; y < sy + h; ++y) {
    for(int x = sx; x < sx + w; ++x)
    {
      if (data[y * image.get_pitch() + 4*x + 3] != 0)
      {
        return false;
      }
    }
  }

  return true;
}

} // namespace

TileFactory::TileFactory(const Pathname& filename) :
  tiles(),
  packers(),
  color_packer(),
  descriptions()
{
  packers.push_back(new TilePacker({1024, 1024}));
  packers.push_back(new TilePacker({1024, 1024}));
  color_packer     = 0;

  ReaderDocument doc = ReaderDocument::from_file(filename.get_sys_path());
  if (doc.get_name() != "windstille-tiles") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille tiles file";
    throw std::runtime_error(msg.str());
  }

  ReaderMapping const& reader = doc.get_mapping();

  ReaderCollection tiles_collection;
  reader.read("tiles", tiles_collection);
  for (ReaderObject const& item : tiles_collection.get_objects())
  {
    if (item.get_name() == "tiles") {
      parse_tiles(item.get_mapping());
    } else if (item.get_name() == "tilegroup") {
      // ignore
    }
  }
}

TileFactory::~TileFactory()
{
  for(Tiles::iterator i = tiles.begin(); i != tiles.end(); ++i)
    delete *i;
  tiles.clear();

  for(std::vector<TileDescription*>::iterator i = descriptions.begin(); i != descriptions.end(); ++i)
    delete *i;
  descriptions.clear();

  for(TilePackers::iterator i = packers.begin(); i != packers.end(); ++i)
    delete *i;
  packers.clear();
}

void
TileFactory::parse_tiles(ReaderMapping const& reader)
{
  descriptions.push_back(new TileDescription(reader));

  TileDescription& desc = *descriptions.back();

  if ((false))
  { // Load all on startup
    desc.load(this);
  }
  else
  { // Load on demand
    for(std::vector<int>::size_type i = 0; i < desc.ids.size(); ++i)
    {
      int& id = desc.ids[i];
      if (id != 0)
      {
        if (id >= int(tiles.size()))
          tiles.resize(id + 1, nullptr);

        delete tiles[id];
        tiles[id] = new Tile(desc.colmap[i]);
        tiles[id]->desc = &desc;
      }
    }
  }
}

void
TileFactory::pack(int id, int colmap, SoftwareSurface const& image, const geom::irect& rect)
{
  if(id < int(tiles.size())
     && tiles[id] != nullptr
     && tiles[id]->desc == nullptr)
  {
    std::cout << "Warning: Duplicate tile id '" << id << "' ignoring" << std::endl;
  }
  else
  {
    if (id >= int(tiles.size()))
      tiles.resize(id + 1, nullptr);

    delete tiles[id];
    tiles[id]  = new Tile(colmap);
    tiles[id]->desc  = nullptr;
    tiles[id]->id    = id;

    if (!surface_empty(image, rect.left(), rect.top(), rect.width(), rect.height()))
    {
      if(packers[color_packer]->is_full())
      {
        packers.push_back(new TilePacker({1024, 1024}));
        color_packer = static_cast<int>(packers.size()) - 1;
      }

      geom::frect uv = packers[color_packer]->pack(image,
                                             rect.left(), rect.top(),
                                             rect.width(), rect.height());
      tiles[id]->uv      = uv;
      tiles[id]->packer  = color_packer;
      tiles[id]->texture = packers[color_packer]->get_texture();
    }
  }
}

Tile*
TileFactory::create(int id)
{
  if(id < 0 || id >= static_cast<int>(tiles.size()))
  {
    return nullptr;
  }
  else
  {
    if (tiles[id] && tiles[id]->desc)
      tiles[id]->desc->load(this);

    return tiles[id];
  }
}

/* EOF */
