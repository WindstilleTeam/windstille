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

#ifndef HEADER_WINDSTILLE_FONT_TTF_FONT_HPP
#define HEADER_WINDSTILLE_FONT_TTF_FONT_HPP

#include <GL/glew.h>
#include <memory>
#include <string>

#include "display/color.hpp"
#include "display/texture.hpp"
#include "math/rect.hpp"
#include "font/no_font_effect.hpp"

class FontEffect;
class TTFFontImpl;

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

class TTFFont
{
private:
  static void init();
  static void deinit();
  friend class TTFFontManager;

public:
  TTFFont(std::filesystem::path const& file, int size, const FontEffect& effect = NoFontEffect());
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

  TexturePtr get_texture() const;

  const TTFCharacter& get_character(int c) const;
  void draw(const Vector2f& pos, const std::string& str, const Color& color = Color(1.0f, 1.0f, 1.0f));
  void draw_center(const Vector2f& pos, const std::string& str, const Color& color = Color(1.0f, 1.0f, 1.0f));

private:
  std::unique_ptr<TTFFontImpl> impl;
};

/**
 * Simple wrapper class to call TTFFont::init()/deinit() so we can
 * use proper RAII.
 */
class TTFFontManager
{
public:
  TTFFontManager();
  ~TTFFontManager();
};

#endif

/* EOF */
