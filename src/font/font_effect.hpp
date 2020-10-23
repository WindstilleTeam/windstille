/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_FONT_FONT_EFFECT_HPP
#define HEADER_WINDSTILLE_FONT_FONT_EFFECT_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "display/software_surface.hpp"

/**
 *  The FontEffect class manages the blitting from a glyph bitmap to
 *  the SDL_Surface, it allows to apply different kinds of effects to
 *  a font, such as adding a border around it.
 */
class FontEffect
{
private:
public:
  FontEffect() {}
  virtual ~FontEffect() {}

  /** Returns the new height of the font after its transformation */
  virtual int get_height(int orig_font_size) const =0;

  virtual int get_glyph_width(int orig_glyph_width) const =0;
  virtual int get_glyph_height(int orig_glyph_height) const =0;

  virtual int get_x_offset(int orig_glyph_offset) const =0;
  virtual int get_y_offset(int orig_glyph_offset) const =0;

  virtual void blit(SoftwareSurface& target, const FT_Bitmap& brush, int x_pos, int y_pos) const =0;
};

#endif

/* EOF */
