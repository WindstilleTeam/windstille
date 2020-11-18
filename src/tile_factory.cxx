//  $Id: tile_factory.cxx,v 1.10 2003/09/22 18:37:05 grumbel Exp $
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

#include <string>
#include <ClanLib/Core/System/system.h>
#include <assert.h>
#include <iostream>
#include "globals.hxx"
#include "string_converter.hxx"
#include "scm_helper.hxx"
#include "tile.hxx"
#include "tile_factory.hxx"

extern CL_ResourceManager* resources;

TileFactory* TileFactory::current_ = 0;

TileFactory::TileFactory (const std::string& filename)
{
  SCM input_stream = scm_open_file(scm_from_utf8_string(filename.c_str()), 
                                   scm_from_utf8_string("r"));
  SCM tree = scm_read(input_stream);
  
  if (!(scm_is_true(scm_symbol_p(scm_car(tree))) && scm_is_true(scm_equal_p(scm_from_utf8_symbol("windstille-tiles"), scm_car(tree)))))
    {
      std::cout << "Not a Windstille Tile File!" << std::endl;
    }
  else
    {
      tree = scm_cdr(tree);

      while (!scm_is_true(scm_null_p(tree)))
        {
          SCM current = scm_car(tree);
          
          if (scm_is_true(scm_pair_p(current)))
            {
              SCM name    = scm_car(current);
              SCM data    = scm_cdr(current);
      
              if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("tile"), name))) 
                {
                  parse_tile(data);
                }
              else
                {
                  std::cout << "WindstilleLevel: Unknown tag: " << scm2string(name) << std::endl;
                }
            }
          else
            {
              std::cout << "WindstilleLevel: Not a pair!"  << std::endl;
            }
          tree = scm_cdr(tree);
        }
    }
}

void
TileFactory::parse_tile(SCM data)
{
  int id;
  std::string image;
  unsigned char colmap[8];
  
  while (!scm_is_true(scm_null_p(data)))
    {
      SCM current = scm_car(data);
          
      if (scm_is_true(scm_pair_p(current)))
        {
          SCM name    = scm_car(current);
          SCM data    = scm_cdr(current);

          if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("id"), name)))
            {
              id = scm_to_int(scm_car(data));
            }
          else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("image"), name)))
            {
              image = scm2string(scm_car(data));
            }
          else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("colmap"), name)))
            {
              colmap[0] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[1] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[2] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[3] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[4] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[5] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[6] = scm_to_int(scm_car(data));
              data = scm_cdr(data);
              colmap[7] = scm_to_int(scm_car(data));
            }
        }
      data = scm_cdr(data);
    }

  if (0) // Debugging code
    {
      std::cout << "Tile: id     = " << id << "\n"
                << "      image  = " << image << "\n"
                << "      colmap = " 
                << int(colmap[0]) << ", "
                << int(colmap[1]) << ", "
                << int(colmap[2]) << ", "
                << int(colmap[3]) << ", "
                << int(colmap[4]) << ", "
                << int(colmap[5]) << ", "
                << int(colmap[6]) << ", "
                << int(colmap[7])
                << std::endl;
    }

  tiles[id] = new Tile(image, colmap);
  tiles[id]->id = id;
}

Tile* 
TileFactory::create (int id)
{
  // id 0 is always the empty tile
  if (id == 0)
    { 
      return 0;
    }
  else
    {
      Tiles::iterator i = tiles.find(id);
      if (i != tiles.end())
        {
          return i->second;
        }
      else
        {
          return 0;
        }
    } 
}

void
TileFactory::init()
{
  assert(current_ == 0);
  current_ = new TileFactory(datadir + "tiles.scm");
}

/** Destroy the default TileFactor*/
void
TileFactory::deinit()
{
  delete current_;
}

/* EOF */
