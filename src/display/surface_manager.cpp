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
#include <SDL_image.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "util/util.hpp"
#include "app/globals.hpp"
#include "texture.hpp"
#include "surface.hpp"
#include "texture_manager.hpp"
#include "physfs/physfs_sdl.hpp"

SurfaceManager* SurfaceManager::current_ = 0;

SurfaceManager::SurfaceManager()
{
  assert(current_ == 0);
  current_ = this;
}

SurfaceManager::~SurfaceManager()
{
  if(debug)
  {
    for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
    {
      //      if (i->second.use_count() > 1)
        std::cerr << "Warning: Surface '" << i->first << "' not released.\n";
    }
  }

  current_ = 0;
}

Surface
SurfaceManager::get(const std::string& filename)
{
  Surfaces::iterator i = surfaces.find(filename);

  if(i != surfaces.end())
    return i->second;

  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  float maxu, maxv;
  Texture texture;
  try
  {
    texture = create_texture(image, maxu, maxv);
  }
  catch(std::exception& e)
  {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  Surface result(texture, Rectf(0, 0, maxu, maxv), image->w, image->h);
  surfaces.insert(std::make_pair(filename, result));
  
  SDL_FreeSurface(image);

  return result;
}

void
SurfaceManager::load_grid(const std::string& filename,
                          std::vector<Surface>& surfaces,
                          int width, int height)
{
  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  float maxu, maxv;

  Texture texture;
  try
  {                                                                       
    texture = create_texture(image, maxu, maxv);
  }
  catch(std::exception& e)
  {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());                                      
  }

  for(int y = 0; y <= image->h - height + 1; y += height)
  {
    for(int x = 0; x <= image->w - width + 1; x += width)
    {
      float s_min_u = maxu * x / static_cast<float>(image->w);
      float s_max_u = (maxu * (x + width)) / static_cast<float>(image->w);
      float s_min_v = maxv * x / static_cast<float>(image->h);
      float s_max_v = (maxv * (x + height)) / static_cast<float>(image->h);

      surfaces.push_back(Surface(texture, Rectf(s_min_u, s_min_v, s_max_u, s_max_v), width, height));
    }
  }
  SDL_FreeSurface(image);
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

Texture
SurfaceManager::create_texture(SDL_Surface* image,
                               float& maxu, float& maxv)
{
  int texture_w = power_of_two(image->w);
  int texture_h = power_of_two(image->h);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
      texture_w, texture_h, 32,
      0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
      texture_w, texture_h, 32,
      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  if(convert == 0) 
  {
    throw std::runtime_error("Couldn't create texture: out of memory");
  }
  SDL_SetAlpha(image, 0, 0);
  SDL_BlitSurface(image, 0, convert, 0);

  Texture texture;

  try 
  {
    texture = Texture(convert);
  }
  catch(...) 
  {
    SDL_FreeSurface(convert);
    throw;
  }
  SDL_FreeSurface(convert);
  
  maxu = static_cast<float>(image->w)/static_cast<float>(texture_w);
  maxv = static_cast<float>(image->h)/static_cast<float>(texture_h);

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


/* EOF */
