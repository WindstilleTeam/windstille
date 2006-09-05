/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <algorithm>
#include "border_font_effect.hpp"

BorderFontEffect::BorderFontEffect(int size_)
  : size(size_)
{
}

BorderFontEffect::~BorderFontEffect()
{
}
  
int
BorderFontEffect::get_height(int orig_font_size) const
{
  return orig_font_size + 2*size ;
}
  
int
BorderFontEffect::get_glyph_width(int orig_glyph_width) const
{
  return orig_glyph_width + 2*size ;
}

int
BorderFontEffect::get_glyph_height(int orig_glyph_height) const
{
  return orig_glyph_height + 2*size ;
}
  
void
BorderFontEffect::blit(SDL_Surface* target, const FT_Bitmap& brush, int x_pos, int y_pos) const
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
            
        target_buf[target_pos + 0] = int(float(y)/brush.width * 255);
        target_buf[target_pos + 1] = int(float(y)/brush.width * 255);
        target_buf[target_pos + 2] = 255;
        target_buf[target_pos + 3] = brush.buffer[brush_pos];
      }
    
  SDL_UnlockSurface(target);

}

/* EOF */
