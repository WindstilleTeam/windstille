/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <geom/rect.hpp>
#include <geom/size.hpp>
#include <surf/fwd.hpp>

#include "display/surface.hpp"

class Texture;
class TexturePackerTexture;

class TexturePacker
{
private:
  typedef std::vector<TexturePackerTexture*> Textures;
  geom::isize     texture_size;
  Textures textures;

public:
  TexturePacker(const geom::isize& texture_size);
  ~TexturePacker();

  SurfacePtr upload(SoftwareSurface const& surface);
  bool allocate(const geom::isize& size, geom::irect& rect, TexturePtr& out_texture);

  void save_all_as_png() const;

private:
  TexturePacker(const TexturePacker&);
  TexturePacker& operator=(const TexturePacker&);
};

#endif

/* EOF */
