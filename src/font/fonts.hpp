/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_FONT_FONTS_HPP
#define HEADER_WINDSTILLE_FONT_FONTS_HPP

#include <memory>

#include <wstdisplay/font/ttf_font.hpp>

namespace windstille {

class Fonts
{
public:
  std::unique_ptr<wstdisplay::TTFFont> ttffont;
  std::unique_ptr<wstdisplay::TTFFont> vera12;
  std::unique_ptr<wstdisplay::TTFFont> vera20;

public:
  Fonts(wstdisplay::TTFFontManager& mgr);
  ~Fonts();
};

} // namespace windstille

#endif

/* EOF */
