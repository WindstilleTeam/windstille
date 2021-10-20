/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_FONT_TTF_FONT_MANAGER_HPP
#define HEADER_WINDSTILLE_FONT_TTF_FONT_MANAGER_HPP

#include <memory>
#include <filesystem>

#include "font/no_font_effect.hpp"

class FontEffect;
class TTFFont;

class TTFFontManager
{
public:
  TTFFontManager();
  ~TTFFontManager();

  std::unique_ptr<TTFFont> create_font(std::filesystem::path const& filename, int size, const FontEffect& effect = NoFontEffect());

  FT_Library get_handle() const { return m_freetype; }

private:
  FT_Library m_freetype;

private:
  TTFFontManager(const TTFFontManager&) = delete;
  TTFFontManager& operator=(const TTFFontManager&) = delete;
};

#endif

/* EOF */
