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

#include "tile/tile_factory.hpp"

#include <stdexcept>
#include <sstream>

#include "tile/tile.hpp"
#include "tile/tile_packer.hpp"
#include "util/sexpr_file_reader.hpp"
#include "display/software_surface.hpp"

/** Check if the given region of the given image is fully transparent */
bool surface_empty(SoftwareSurfacePtr image, int sx, int sy, int w, int h)
{
  unsigned char* data = static_cast<unsigned char*>(image->get_pixels());

  for(int y = sy; y < sy + h; ++y)
    for(int x = sx; x < sx + w; ++x)
    {
      if (data[y * image->get_pitch() + 4*x + 3] != 0)
      {
        return false;
      }
    }

  return true;
}

TileFactory::TileFactory(const Pathname& filename) :
  tiles(),
  packers(),
  color_packer(),
  descriptions()
{
  packers.push_back(new TilePacker(1024, 1024));
  packers.push_back(new TilePacker(1024, 1024));
  color_packer     = 0;

  FileReader reader = FileReader::parse(filename);
  if(reader.get_name() != "windstille-tiles")
  {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille tiles file";
    throw std::runtime_error(msg.str());
  }

  std::vector<FileReader> sections = reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
  {
    if (i->get_name() == "tiles") {
      parse_tiles(*i);
    } else if (i->get_name() == "tilegroup") {
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
TileFactory::parse_tiles(FileReader& reader)
{
  descriptions.push_back(new TileDescription(reader));

  TileDescription& desc = *descriptions.back();

  if (0)
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
          tiles.resize(id + 1, 0);

        delete tiles[id];
        tiles[id] = new Tile(desc.colmap[i]);
        tiles[id]->desc = &desc;
      }
    }
  }
}

void
TileFactory::pack(int id, int colmap, SoftwareSurfacePtr image, const Rect& rect)
{
  if(id < int(tiles.size())
     && tiles[id] != 0
     && tiles[id]->desc == 0)
  {
    std::cout << "Warning: Duplicate tile id '" << id << "' ignoring" << std::endl;
  }
  else
  {
    if (id >= int(tiles.size()))
      tiles.resize(id + 1, 0);

    delete tiles[id];
    tiles[id]  = new Tile(colmap);
    tiles[id]->desc  = 0;
    tiles[id]->id    = id;

    if (!surface_empty(image, rect.left, rect.top, rect.get_width(), rect.get_height()))
    {
      if(packers[color_packer]->is_full())
      {
        packers.push_back(new TilePacker(1024, 1024));
        color_packer = static_cast<int>(packers.size()) - 1;
      }

      Rectf uv = packers[color_packer]->pack(image,
                                             rect.left, rect.top,
                                             rect.get_width(), rect.get_height());
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
    return 0;
  }
  else
  {
    if (tiles[id] && tiles[id]->desc)
      tiles[id]->desc->load(this);

    return tiles[id];
  }
}

/* EOF */
