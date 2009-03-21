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
#include <GL/glew.h>
#include <GL/gl.h>
#include "util/util.hpp"
#include "app/globals.hpp"
#include "texture.hpp"
#include "software_surface.hpp"
#include "physfs/physfs_sdl.hpp"

TextureManager* texture_manager = 0;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
#if 0
  if(debug)
  {
    for(Textures::iterator i = textures.begin(); i != textures.end(); ++i) {
      std::cerr << "Warning: Texture '" << i->first << "' not released.\n";
    }
  }
#endif
}

Texture
TextureManager::get(const std::string& filename)
{
  Textures::iterator i = textures.find(filename);
  if(i != textures.end())
    {
      return i->second;
    }
  else
    {
      try 
        {
          SoftwareSurface image(filename);
          Texture texture(image);

          textures.insert(std::make_pair(filename, texture));

          return texture;
        } 
      catch(std::exception& e) 
        {
          std::cerr << e.what() << std::endl;
          return get("images/404.png");
        }
    }
}

/* EOF */
