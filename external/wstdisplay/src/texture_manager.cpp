// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "texture_manager.hpp"

#include <iostream>

#include "texture.hpp"
#include "software_surface.hpp"

namespace wstdisplay {

TextureManager::TextureManager() :
  textures(),
  m_fallback()
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

void
TextureManager::set_fallback(std::filesystem::path const& filename)
{
  m_fallback = get(filename);
}

TexturePtr
TextureManager::get(std::filesystem::path const& filename)
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
      SoftwareSurface image = SoftwareSurface::from_file(filename);
      TexturePtr texture = Texture::create(image);

      textures.insert(std::make_pair(filename, texture));

      return texture;
    }
    catch(std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return m_fallback;
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

} // namespace wstdisplay

/* EOF */
