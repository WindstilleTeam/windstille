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

#ifndef HEADER_WINDSTILLE_FONT_NO_FONT_EFFECT_HPP
#define HEADER_WINDSTILLE_FONT_NO_FONT_EFFECT_HPP

#include "font/font_effect.hpp"

class NoFontEffect : public FontEffect
{
private:
public:
  NoFontEffect() {}
  ~NoFontEffect() {}

  int get_height(int orig_font_size) const { return orig_font_size; }
  
  int get_glyph_width(int orig_glyph_width) const   { return orig_glyph_width; }
  int get_glyph_height(int orig_glyph_height) const { return orig_glyph_height; }

  int get_x_offset(int orig_glyph_offset) const { return orig_glyph_offset; }
  int get_y_offset(int orig_glyph_offset) const { return orig_glyph_offset; }
  
  void blit(const SoftwareSurface& target, const FT_Bitmap& brush, int x_pos, int y_pos) const;
  
  /* disabled for g++-4.2 compatibilty
private:
  NoFontEffect (const NoFontEffect&);
  NoFontEffect& operator= (const NoFontEffect&);
  */
};

#endif

/* EOF */
