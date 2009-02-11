/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_TEXTURE_MANAGER_HPP
#define HEADER_WINDSTILLE_DISPLAY_TEXTURE_MANAGER_HPP

#include <string>
#include <map>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

class Texture;

/**
 * This class manages a map of image files to OpenGL textures.
 */
class TextureManager
{
public:
  TextureManager();
  ~TextureManager();

  /**
   * returns a texture containing the image specified with filename
   * The TextureManager keeps track of loaded Textures and might return a
   * reference to an existing texture.
   * Only textures with power of 2 dimensions are supported here. Use
   * SurfaceManager for images with other dimensions.
   * Note: Texture is a refcounted class, store it with Ref<Texture>
   */
  Texture get(const std::string& filename);

private:
  typedef std::map<std::string, Texture> Textures;
  Textures textures;
};

extern TextureManager* texture_manager;

#endif

/* EOF */
