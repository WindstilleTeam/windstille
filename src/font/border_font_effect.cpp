/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <algorithm>
#include "display/software_surface.hpp"
#include "border_font_effect.hpp"

BorderFontEffect::BorderFontEffect(int size_, bool outline_)
  : size(size_),
    outline(outline_)
{
}

BorderFontEffect::~BorderFontEffect()
{
}
  
int
BorderFontEffect::get_height(int orig_font_size) const
{
  return orig_font_size + 2*size;
}
  
int
BorderFontEffect::get_glyph_width(int orig_glyph_width) const
{
  return orig_glyph_width + 2*size;
}

int
BorderFontEffect::get_glyph_height(int orig_glyph_height) const
{
  return orig_glyph_height + 2*size;
}
  
int
BorderFontEffect::get_x_offset(int orig_glyph_offset) const
{
  return orig_glyph_offset - size;
}

int
BorderFontEffect::get_y_offset(int orig_glyph_offset) const
{
  return orig_glyph_offset - size;
}

void
BorderFontEffect::blit(const SoftwareSurface& target, const FT_Bitmap& brush, int x_pos, int y_pos) const
{
  x_pos += size;
  y_pos += size;

  int start_x = std::max(0, -x_pos);
  int start_y = std::max(0, -y_pos);
  
  int end_x = std::min(brush.width, target.get_width()  - x_pos);
  int end_y = std::min(brush.rows,  target.get_height() - y_pos);

  unsigned char* target_buf = static_cast<unsigned char*>(target.get_pixels());

  int target_pitch = target.get_pitch();

  uint8_t red   = 0;
  uint8_t blue  = 0;
  uint8_t green = 0;

  if (!outline)
    {
      red = blue = green = 255;
    }

  // Draw the border
  for (int y = start_y; y < end_y; ++y)
    for (int x = start_x; x < end_x; ++x)
      {
        for(int by = -size; by <= size; ++by)
          for(int bx = -size+abs(by); bx <= size-abs(by); ++bx)
            {
              int target_pos = (y + y_pos + by) * target_pitch + 4*(x + x_pos + bx);
              int brush_pos  = y * brush.pitch + x;
            
              target_buf[target_pos + 0] = red;
              target_buf[target_pos + 1] = blue;
              target_buf[target_pos + 2] = green;
              target_buf[target_pos + 3] = std::min(target_buf[target_pos + 3] + brush.buffer[brush_pos], 255);
            }
      }

  if (outline)
    {
      // Draw the font itself
      for (int y = start_y; y < end_y; ++y)
        for (int x = start_x; x < end_x; ++x)
          {
            int target_pos = (y + y_pos) * target_pitch + 4*(x + x_pos);
            int brush_pos  = y * brush.pitch + x;
        
            int alpha = brush.buffer[brush_pos];

            target_buf[target_pos + 0] = std::min((target_buf[target_pos + 0] * (255 - alpha) + alpha * 255)/255, 255);
            target_buf[target_pos + 1] = std::min((target_buf[target_pos + 1] * (255 - alpha) + alpha * 255)/255, 255);
            target_buf[target_pos + 2] = std::min((target_buf[target_pos + 2] * (255 - alpha) + alpha * 255)/255, 255);
            target_buf[target_pos + 3] = std::min(target_buf[target_pos + 3] + brush.buffer[brush_pos], 255);
          }
    }
}  

/* EOF */
