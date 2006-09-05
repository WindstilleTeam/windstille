/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_TILE_PACKER_HXX
#define HEADER_TILE_PACKER_HXX

#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "display/texture.hpp"
#include "math/rect.hpp"

class TilePackerImpl;

/** Creates a pixelbuffer of the given size and packs 32x32 large
    tiles into it for later conversion to a texture */
class TilePacker
{
private:
public:
  /** Let the texture be width/height large (only power of two values
      recomment) */
  TilePacker(int width, int height);
  ~TilePacker();

  /** Pack a tile and return the position where it is placed in the
      pixel buffer */
  Rectf pack(SDL_Surface* image, int x, int y, int w, int h);

  /** Return true if the PixelBuffer is full */
  bool is_full() const;

  Texture get_texture() const;

private:
  TilePackerImpl* impl;
};

#endif

/* EOF */
