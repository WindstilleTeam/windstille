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

#include "texture_manager.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "util/util.hpp"
#include "globals.hpp"
#include "texture.hpp"
#include "physfs/physfs_sdl.hpp"

TextureManager* texture_manager = 0;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
  if(debug)
  {
    for(Textures::iterator i = textures.begin(); i != textures.end(); ++i) {
      std::cerr << "Warning: Texture '" << i->first << "' not released.\n";
    }
  }
}

Texture
TextureManager::get(const std::string& filename)
{
  Textures::iterator i = textures.find(filename);
  if(i != textures.end())
    return i->second;

  SDL_Surface* image = 0;
  try {
    image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
    if(!image) {
      std::ostringstream msg;
      msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    return get("images/404.png");
  }

  Texture texture;
  try {
    texture = Texture(image);
  } catch(std::exception& e) {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  SDL_FreeSurface(image);
  
  textures.insert(std::make_pair(filename, texture));
  return texture;
}

/* EOF */
