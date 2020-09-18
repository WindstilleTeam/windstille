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

#include "display/texture_manager.hpp"

#include <iostream>

#include "display/texture.hpp"
#include "display/software_surface.hpp"

TextureManager::TextureManager() :
  textures()
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

TexturePtr
TextureManager::get(const Pathname& filename)
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
      SoftwareSurfacePtr image = SoftwareSurface::create(filename);
      TexturePtr texture = Texture::create(image);

      textures.insert(std::make_pair(filename, texture));

      return texture;
    }
    catch(std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return get(Pathname("images/404.png"));
    }
  }
}

void
TextureManager::cleanup()
{
  //std::cout << "TextureManager: size: " << textures.size() << std::endl;
  for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
  {
    std::cout << "TextureManager: use count: " << i->second.use_count() << std::endl;
    if (i->second.use_count() == 1)
    {
      //std::cout << "TextureManager: erasing a texture" << std::endl;
      textures.erase(i);
    }
  }
}

/* EOF */
