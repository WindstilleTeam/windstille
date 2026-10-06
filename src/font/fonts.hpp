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

#include <wstdisplay/font/font.hpp>
#include <wstdisplay/font/font_manager.hpp>

namespace windstille {

/** The fonts used by the game, loaded once at startup */
class Fonts
{
public:
  explicit Fonts(wstdisplay::Device& device);
  ~Fonts();

  wstdisplay::FontManager& get_manager() { return m_manager; }

private:
  wstdisplay::FontManager m_manager;

public:
  wstdisplay::Font const* ttffont;
  wstdisplay::Font const* vera12;
  wstdisplay::Font const* vera20;

public:
  Fonts(Fonts const&) = delete;
  Fonts& operator=(Fonts const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
