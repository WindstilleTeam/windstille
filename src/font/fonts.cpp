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

#include "app/globals.hpp"
#include "border_font_effect.hpp"
#include "fonts.hpp"

TTFFont* Fonts::ttffont = 0;
TTFFont* Fonts::vera12  = 0;
TTFFont* Fonts::vera16  = 0;
TTFFont* Fonts::vera20  = 0;
TTFFont* Fonts::vera28  = 0;

void
Fonts::init()
{
  std::auto_ptr<BorderFontEffect> border_effect(new BorderFontEffect(1));

  ttffont = new TTFFont("fonts/VeraMono.ttf", 12);
  vera12  = new TTFFont("fonts/Vera.ttf", 12);
  vera16  = new TTFFont("fonts/Vera.ttf", 16);
  vera20  = new TTFFont("fonts/Vera.ttf", 20);
  vera28  = new TTFFont("fonts/Vera.ttf", 28);
}

void
Fonts::deinit()
{
  delete ttffont;
  delete vera12;
  delete vera16;
  delete vera20;
  delete vera28;
}

/* EOF */
