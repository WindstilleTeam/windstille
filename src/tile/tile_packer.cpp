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

#include <GL/glew.h>
#include <SDL.h>

#include "app/globals.hpp"
#include "tile/tile_packer.hpp"
#include "display/assert_gl.hpp"
#include "display/blitter.hpp"
#include "display/software_surface.hpp"

class TilePackerImpl
{
public:
  // Position for the next tile
  int x_pos;
  int y_pos;

  TexturePtr texture;

  geom::isize size;

  TilePackerImpl()
    : x_pos(),
      y_pos(),
      texture(),
      size()
  {}
};

TilePacker::TilePacker(geom::isize const& size) :
  impl(new TilePackerImpl())
{
  impl->x_pos = 0;
  impl->y_pos = 0;

  impl->size = size;

  impl->texture = Texture::create(GL_TEXTURE_2D, size);

  assert_gl();
}

TilePacker::~TilePacker()
{
}

/** Pack a tile and return the position where it is placed in the
    pixel buffer */
geom::frect
TilePacker::pack(SoftwareSurfacePtr image, int x, int y, int w, int h)
{
  assert(w == TILE_RESOLUTION && h == TILE_RESOLUTION);
  assert(!is_full());

  SoftwareSurfacePtr convert = SoftwareSurface::create(SoftwareSurface::RGBA, {w + 2, h + 2});

  SDL_Rect source_rect;
  source_rect.x = static_cast<Sint16>(x);
  source_rect.y = static_cast<Sint16>(y);
  source_rect.w = static_cast<Sint16>(w);
  source_rect.h = static_cast<Sint16>(h);

  SDL_Rect dest_rect;
  dest_rect.x = static_cast<Sint16>(1);
  dest_rect.y = static_cast<Sint16>(1);
  dest_rect.w = static_cast<Sint16>(w);
  dest_rect.h = static_cast<Sint16>(h);

  SDL_BlitSurface(image->get_surface(), &source_rect, convert->get_surface(), &dest_rect);

  generate_border(convert, 1, 1, TILE_RESOLUTION, TILE_RESOLUTION);

  impl->texture->put(convert, impl->x_pos, impl->y_pos);

  assert_gl();

  geom::frect rect(geom::fpoint(static_cast<float>(impl->x_pos + 1) / static_cast<float>(impl->size.width()),
                                static_cast<float>(impl->y_pos + 1) / static_cast<float>(impl->size.height())),
                   geom::fsize(static_cast<float>(TILE_RESOLUTION) / static_cast<float>(impl->size.width()),
                               static_cast<float>(TILE_RESOLUTION) / static_cast<float>(impl->size.height())));

  // we move by TILE_RESOLUTION+1 to avoid tiles bleeding into each other
  // when blending
  impl->x_pos += TILE_RESOLUTION + 2;
  if (impl->x_pos + TILE_RESOLUTION > impl->size.width())
  {
    impl->x_pos = 0;
    impl->y_pos += TILE_RESOLUTION + 2;
  }

  return rect;
}

/** Return true if the PixelBuffer is full */
bool
TilePacker::is_full() const
{
  return (impl->y_pos + TILE_RESOLUTION + 2 > impl->size.height());
}

TexturePtr
TilePacker::get_texture() const
{
  return impl->texture;
}

/* EOF */
