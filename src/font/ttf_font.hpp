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

#ifndef HEADER_TTF_FONT_HXX
#define HEADER_TTF_FONT_HXX

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include "color.hpp"
#include "math/rect.hpp"
#include "display/texture.hpp"

class FontEffect;

class TTFCharacter
{
public:
  /** The position of the image, relative to the current cursor
      position in screen coordinates */
  Rect pos;

  /** The position of the character in a OpenGL texture, given in
      uv-coordinates */
  Rectf uv;

  /** Amount of pixels that the cursor will advance after having
      printed this character */
  int advance;

  TTFCharacter(const Rect& pos, const Rectf& uv, int advance);
};

class TTFFontImpl;

/** */
class TTFFont
{
public:
  static void init();
  static void deinit();

  TTFFont(const std::string& file, int size, FontEffect* effect = 0);
  ~TTFFont();

  /** */
  int get_height() const;

  /** Returns the width of a given piece of text, doesn't take
      newlines into account */
  int get_width(const std::string& text) const;

  /** Returns the height as given in the constructor, this does *not*
      take into account any possible resize effects done by
      FontEffoct */
  int get_size() const;

  Texture get_texture() const;

  const TTFCharacter& get_character(int c) const;
  void draw(float x_pos, float y_pos, const std::string& str, const Color& color = Color(1.0f, 1.0f, 1.0f));
  void draw_center(float x_pos, float y_pos, const std::string& str, const Color& color = Color(1.0f, 1.0f, 1.0f));
private:
  TTFFontImpl* impl;
};

#endif

/* EOF */
