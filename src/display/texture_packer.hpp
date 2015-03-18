/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_TEXTURE_PACKER_HPP
#define HEADER_WINDSTILLE_DISPLAY_TEXTURE_PACKER_HPP

#include <vector>

#include "math/rect.hpp"
#include "math/size.hpp"

#include "display/surface.hpp"

class SoftwareSurface;
class Texture;
class TexturePackerTexture;

class TexturePacker
{
private:
  typedef std::vector<TexturePackerTexture*> Textures;
  Size     texture_size;
  Textures textures;

public:
  TexturePacker(const Size& texture_size);
  ~TexturePacker();

  SurfacePtr upload(SoftwareSurfacePtr surface);
  bool allocate(const Size& size, Rect& rect, TexturePtr& out_texture);

  void save_all_as_png() const;

private:
  TexturePacker(const TexturePacker&);
  TexturePacker& operator=(const TexturePacker&);
};

#endif

/* EOF */
