/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>

#include "app/globals.hpp"
#include "font/border_font_effect.hpp"
#include "font/fonts.hpp"
#include "util/pathname.hpp"

Fonts* Fonts::s_current = 0;

Fonts::Fonts()
  : ttffont(new TTFFont(Pathname("fonts/VeraMono.ttf"), 14, BorderFontEffect(1, true))),
    vera12(new TTFFont(Pathname("fonts/Vera.ttf"),     12, BorderFontEffect(2, true))),
    vera20(new TTFFont(Pathname("fonts/Vera.ttf"),     20, BorderFontEffect(2, true)))
{
  assert(!s_current);
  s_current = this;
}

Fonts::~Fonts()
{
}

/* EOF */
