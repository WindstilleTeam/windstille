/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "display/software_surface.hpp"
#include "font/no_font_effect.hpp"

void
NoFontEffect::blit(SoftwareSurface& target, const FT_Bitmap& brush, int x_pos, int y_pos) const
{
  int start_x = std::max(0, -x_pos);
  int start_y = std::max(0, -y_pos);

  int end_x = std::min(static_cast<int>(brush.width), target.get_width()  - x_pos);
  int end_y = std::min(static_cast<int>(brush.rows), target.get_height() - y_pos);

  unsigned char* target_buf = target.get_data();

  int target_pitch = target.get_pitch();

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
}

/* EOF */
