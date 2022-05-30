/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_TILE_TILE_PACKER_HPP
#define HEADER_WINDSTILLE_TILE_TILE_PACKER_HPP

#include <GL/glew.h>
#include <memory>

#include <geom/geom.hpp>
#include <surf/fwd.hpp>

#include <wstdisplay/texture.hpp>

class TilePackerImpl;

/** Creates a pixelbuffer of the given size and packs 32x32 large
    tiles into it for later conversion to a texture */
class TilePacker
{
private:
public:
  /** Let the texture be width/height large (only power of two values
      recomment) */
  TilePacker(geom::isize const& size);
  ~TilePacker();

  /** Pack a tile and return the position where it is placed in the
      pixel buffer */
  geom::frect pack(surf::SoftwareSurface const& image, int x, int y, int w, int h);

  /** Return true if the PixelBuffer is full */
  bool is_full() const;

  wstdisplay::TexturePtr get_texture() const;

private:
  std::unique_ptr<TilePackerImpl> impl;

private:
  TilePacker(TilePacker const&);
  TilePacker& operator=(TilePacker const&);
};

#endif

/* EOF */
