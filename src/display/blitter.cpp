//
//
//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <assert.h>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include "blitter.hpp"

void generate_border(SDL_Surface* surface,
                     int x_pos, int y_pos, int width, int height)
{
  assert(surface->format->BitsPerPixel == 32);
  SDL_LockSurface(surface);
 
  uint8_t* data = static_cast<uint8_t*>(surface->pixels);
  int pitch = surface->pitch;

  // duplicate the top line
  memcpy(data + (y_pos-1)*pitch + 4*x_pos, 
         data + (y_pos)*pitch + 4*x_pos,
         4*width);
  // duplicate the bottom line
  memcpy(data + (y_pos+height)*pitch + 4*x_pos, 
         data + (y_pos+height-1)*pitch + 4*x_pos,  
         4*width);

  // duplicate left and right borders
  for(int y = y_pos-1; y < y_pos + height+1; ++y)
    {
      uint32_t* p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos-1)));
      *p = *(p+1);
      p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos + width)));
      *p = *(p-1);
    }

  SDL_UnlockSurface(surface);
}

void 
blit_ftbitmap(SDL_Surface* target, const FT_Bitmap& brush, int x_pos, int y_pos)
{
  SDL_LockSurface(target);
  
  int start_x = std::max(0, -x_pos);
  int start_y = std::max(0, -y_pos);
  
  int end_x = std::min(brush.width, target->w  - x_pos);
  int end_y = std::min(brush.rows,  target->h - y_pos);

  unsigned char* target_buf = static_cast<unsigned char*>(target->pixels);

  int target_pitch = target->pitch;

  for (int y = start_y; y < end_y; ++y)
    for (int x = start_x; x < end_x; ++x)
      {
        int target_pos = (y + y_pos) * target_pitch + 4*(x + x_pos);
        int brush_pos  = y * brush.pitch + x;
            
        target_buf[target_pos + 0] = 255;
        target_buf[target_pos + 1] = 255;
        target_buf[target_pos + 2] = 255;
        target_buf[target_pos + 3] = brush.buffer[brush_pos];
      }
    
  SDL_UnlockSurface(target);
}

SDL_Surface* create_surface_rgba(int width, int height)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* pixelbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                  1024, 1024, 32,
                                                  0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* pixelbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                  1024, 1024, 32,
                                                  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
  return pixelbuffer;
}

/* EOF */
