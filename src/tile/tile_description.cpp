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

#include <sstream>
#include <stdexcept>

#include "app/globals.hpp"
#include "display/software_surface.hpp"
#include "math/rect.hpp"
#include "tile/tile_factory.hpp"

TileDescription::TileDescription(FileReader& props)
 : ids(),
   colmap(),
   filename(),
   width(0), 
   height(0)
{
  props.get("ids",    ids);
  props.get("image",  filename);
  props.get("colmap", colmap);
  props.get("width",  width);
  props.get("height", height);
  
  if(filename == "")
    throw std::runtime_error("Missing color-image");
}

void
TileDescription::load(TileFactory* factory)
{  
  // FIXM: SoftwareSurface image(Pathname(filename)); doesn't work, as its handled as function declaration!?
  SoftwareSurface image = SoftwareSurface(Pathname(filename));

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

/* EOF */
