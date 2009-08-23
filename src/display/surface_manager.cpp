/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#include "surface_manager.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <GL/glew.h>
#include <GL/gl.h>

#include "app/globals.hpp"
#include "math/math.hpp"
#include "physfs/physfs_sdl.hpp"
#include "software_surface.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "texture_manager.hpp"
#include "texture_packer.hpp"
#include "util/util.hpp"


SurfaceManager::SurfaceManager()
  : texture_packer(new TexturePacker(Size(2048, 2048))),
    surfaces()
{
}

SurfaceManager::~SurfaceManager()
{
#if 0 
  if (debug)
    {
      for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
        {
          //      if (i->second.use_count() > 1)
          std::cerr << "Warning: Surface '" << i->first << "' not released.\n";
        }
    }
#endif
}

Surface
SurfaceManager::get(const Pathname& filename)
{
  Surfaces::iterator i = surfaces.find(filename);

  if(i != surfaces.end())
    { // Surface in cache, return it
      return i->second;
    }
  else
    {
      SoftwareSurface image(filename);

      if (1)
        {
          Surface result = texture_packer->upload(image);
          surfaces.insert(std::make_pair(filename, result));
          return result;              
        }
      else
        {
          float maxu, maxv;
          Texture texture;

          try
            {
              texture = create_texture(image, maxu, maxv);
            }
          catch(std::exception& e)
            {
              std::ostringstream msg;
              msg << "Couldn't create texture for '" << filename << "': " << e.what();
              throw std::runtime_error(msg.str());
            }
        
          Surface result(texture, Rectf(0, 0, maxu, maxv), Sizef(image.get_width(), image.get_height()));
          surfaces.insert(std::make_pair(filename, result));
          return result;
        }      
    }
}

void
SurfaceManager::load_grid(const Pathname& filename,
                          std::vector<Surface>& surfaces,
                          int width, int height)
{
  SoftwareSurface image(filename);
  float maxu, maxv;

  Texture texture;

  try
    {                                                                       
      texture = create_texture(image, maxu, maxv);
    }
  catch(std::exception& e)
    {
      std::ostringstream msg;
      msg << "Couldn't create texture for '" << filename << "': " << e.what();
      throw std::runtime_error(msg.str());                                      
    }

  for(int y = 0; y <= image.get_height() - height + 1; y += height)
    {
      for(int x = 0; x <= image.get_width() - width + 1; x += width)
        {
          float s_min_u = maxu * x / static_cast<float>(image.get_width());
          float s_max_u = (maxu * (x + width)) / static_cast<float>(image.get_width());
          float s_min_v = maxv * x / static_cast<float>(image.get_height());
          float s_max_v = (maxv * (x + height)) / static_cast<float>(image.get_height());

          surfaces.push_back(Surface(texture, Rectf(s_min_u, s_min_v, s_max_u, s_max_v), Sizef(width, height)));
        }
    }
}

Texture
SurfaceManager::create_texture(const SoftwareSurface& image,
                               float& maxu, float& maxv)
{
  int texture_w = math::round_to_power_of_two(image.get_width());
  int texture_h = math::round_to_power_of_two(image.get_height());

  SoftwareSurface convert(texture_w, texture_h);

  image.blit(convert, 0, 0);

  Texture texture = Texture(convert);
  
  maxu = static_cast<float>(image.get_width())/static_cast<float>(texture_w);
  maxv = static_cast<float>(image.get_height())/static_cast<float>(texture_h);

  return texture;
}

void
SurfaceManager::cleanup()
{
  for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
    {
      if (i->second.use_count() == 1)
        {
          surfaces.erase(i);
        }
    }
}

void
SurfaceManager::save_all_as_png() const
{
  texture_packer->save_all_as_png();
}

/* EOF */
