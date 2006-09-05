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
#include <config.h>

#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <memory>
#include "globals.hpp"
#include "tile.hpp"
#include "tile_packer.hpp"
#include "tile_factory.hpp"
#include "tile_description.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/properties.hpp"
#include "display/surface_manager.hpp"
#include "display/texture.hpp"
#include "physfs/physfs_sdl.hpp"

TileFactory* TileFactory::current_ = 0;

std::string TileFactory::tile_def_file = "tiles.scm";

/** Check if the given region of the given image is fully transparent */
bool surface_empty(SDL_Surface* image, int sx, int sy, int w, int h)
{
  SDL_LockSurface(image);
  
  unsigned char* data = static_cast<unsigned char*>(image->pixels);
  
  for(int y = sy; y < sy + h; ++y)
    for(int x = sx; x < sx + w; ++x)
      {
        if (data[y * image->pitch + 4*x + 3] != 0)
          { 
            SDL_UnlockSurface(image);
            return false;
          }
      }

  SDL_UnlockSurface(image);
  return true;
}

TileFactory::TileFactory (const std::string& filename)
{
  using namespace lisp;

  packers.push_back(new TilePacker(1024, 1024));
  packers.push_back(new TilePacker(1024, 1024));
  color_packer     = 0;

  std::auto_ptr<Lisp> root (Parser::parse(filename));
  Properties rootp(root.get());
  
  const lisp::Lisp* tiles_lisp = 0;
  if(rootp.get("windstille-tiles", tiles_lisp) == false) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille tiles file";
    throw std::runtime_error(msg.str());
  }
  
  Properties props(tiles_lisp);
  PropertyIterator<const lisp::Lisp*> iter;
  props.get_iter("tiles", iter);
  while(iter.next()) {
    parse_tiles(*iter);
  }
  props.print_unused_warnings("windstille-tiles");
}

TileFactory::~TileFactory()
{
  for(Tiles::iterator i = tiles.begin(); i != tiles.end(); ++i)
    {
      delete *i;
    }
  tiles.clear();

  for(std::vector<TileDescription*>::iterator i = descriptions.begin(); i != descriptions.end(); ++i)
    {
      delete *i;
    }
}

void
TileFactory::parse_tiles(const lisp::Lisp* data)
{
  using namespace lisp;
  assert(data);

  lisp::Properties props(data);

  descriptions.push_back(new TileDescription(props));
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
TileFactory::pack(int id, int colmap, SDL_Surface* image, const Rect& rect)
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
              color_packer = packers.size() - 1;
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
  if(id < 0 || id >= (int) tiles.size())
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

void
TileFactory::init()
{
  assert(current_ == 0);
  current_ = new TileFactory(tile_def_file);
}

/** Destroy the default TileFactor */
void
TileFactory::deinit()
{
  delete current_;
}

/* EOF */
