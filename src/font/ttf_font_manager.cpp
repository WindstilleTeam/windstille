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

#include "font/ttf_font_manager.hpp"

#include <ft2build.h>

#include "font/ttf_font.hpp"

TTFFontManager::TTFFontManager() :
  m_freetype()
{
  FT_Error error;

  error = FT_Init_FreeType(&m_freetype);
  if (error) {
    throw std::runtime_error( "could not initialize FreeType" );
  }
}

TTFFontManager::~TTFFontManager()
{
  FT_Done_FreeType(m_freetype);
}

std::unique_ptr<TTFFont>
TTFFontManager::create_font(std::filesystem::path const& filename, int size, const FontEffect& effect)
{
  return std::make_unique<TTFFont>(*this, filename, size, effect);
}

/* EOF */
