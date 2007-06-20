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

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL.h>
#include <SDL_image.h>
#include "physfs/physfs_sdl.hpp"
#include "globals.hpp"
#include "tile.hpp"
#include "tile_packer.hpp"
#include "tile_factory.hpp"
#include "tile_description.hpp"

TileDescription::TileDescription(FileReader& props)
  : width(0), height(0)
{
  props.get("ids",    ids);
  props.get("image",  filename);
  props.get("colmap", colmap);
  props.get("width",  width);
  props.get("height", height);
    
  props.print_unused_warnings("tiles");

  if(filename == "")
    throw std::runtime_error("Missing color-image");
}

void
TileDescription::load(TileFactory* factory)
{  
  if (debug)
    std::cout << "Loading tiles: " << filename << std::endl;

  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image) 
    {
      std::ostringstream msg;
      msg << "Couldn't load image '" << filename << "': " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
  else
    {
      try 
        {
          int num_tiles = width * height; //(image->w/TILE_RESOLUTION) * (image->h/TILE_RESOLUTION);
          if (int(colmap.size()) != num_tiles)
            {
              std::ostringstream str;
              str << "'colmap' information and num_tiles mismatch (" 
                  << colmap.size() << " != " << num_tiles << ") for image '" << filename << "'";
              throw std::runtime_error(str.str());
            }

          if (int(ids.size()) != num_tiles)
            {
              std::ostringstream str;
              str << "'ids' information and num_tiles mismatch (" 
                  << ids.size() << " != " << num_tiles << ") for image '" << filename << "'";
              throw std::runtime_error(str.str());
            }
    
          int i = 0;
          for (int y = 0; y < height*TILE_RESOLUTION; y += TILE_RESOLUTION)
            {
              for (int x = 0; x < width*TILE_RESOLUTION; x += TILE_RESOLUTION)
                {
                  if(ids[i] != -1)
                    {
                      factory->pack(ids[i], colmap[i], image,
                                    Rect(x, y, x+TILE_RESOLUTION, y+TILE_RESOLUTION));
                    }

                  i += 1; 
                }
            }
        } 
      catch(...) 
        {
          SDL_FreeSurface(image);
          throw;
        }
      SDL_FreeSurface(image);
    }
}

/* EOF */
