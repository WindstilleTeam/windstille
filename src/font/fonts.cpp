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

#include "util/pathname.hpp"

#include "font/border_font_effect.hpp"
#include "font/ttf_font_manager.hpp"
#include "font/fonts.hpp"

Fonts::Fonts(TTFFontManager& mgr) :
  ttffont(mgr.create_font(Pathname("fonts/VeraMono.ttf"), 14, BorderFontEffect(1, true))),
  vera12(mgr.create_font(Pathname("fonts/Vera.ttf"), 12, BorderFontEffect(2, true))),
  vera20(mgr.create_font(Pathname("fonts/Vera.ttf"), 20, BorderFontEffect(2, true)))
{
}

Fonts::~Fonts()
{
}

/* EOF */
