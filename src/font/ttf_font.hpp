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

#include <geom/geom.hpp>

#include <surf/color.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/texture.hpp>
#include "font/no_font_effect.hpp"

class FontEffect;
class TTFFontImpl;
class TTFFontManager;

class TTFCharacter
{
public:
  /** The position of the image, relative to the current cursor
      position in screen coordinates */
  geom::irect pos;

  /** The position of the character in a OpenGL texture, given in
      uv-coordinates */
  geom::frect uv;

  /** Amount of pixels that the cursor will advance after having
      printed this character */
  int advance;

  TTFCharacter(const geom::irect& pos, const geom::frect& uv, int advance);
};

class TTFFont
{
public:
  TTFFont(TTFFontManager& mgr, std::filesystem::path const& file, int size, const FontEffect& effect = NoFontEffect());
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

  wstdisplay::TexturePtr get_texture() const;

  const TTFCharacter& get_character(int c) const;
  void draw(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, const std::string& str, const surf::Color& color = surf::Color(1.0f, 1.0f, 1.0f));
  void draw_center(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, const std::string& str, const surf::Color& color = surf::Color(1.0f, 1.0f, 1.0f));
  void draw(wstdisplay::DrawingContext& ctx, const std::string& text, float x, float y, float z);

private:
  std::unique_ptr<TTFFontImpl> impl;
};

#endif

/* EOF */
