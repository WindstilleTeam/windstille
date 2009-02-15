/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_SCREEN_TITLE_SCREEN_HPP
#define HEADER_WINDSTILLE_SCREEN_TITLE_SCREEN_HPP

#include "sprite2d/sprite.hpp"
#include "screen.hpp"

/** */
class TitleScreen : public Screen
{
private:
  Sprite background;

public:
  TitleScreen();
  ~TitleScreen();

  void on_startup();

  void draw();
  void update(float delta, const Controller& controller);

  void handle_event(const SDL_Event& );

private:
  TitleScreen (const TitleScreen&);
  TitleScreen& operator= (const TitleScreen&);
};

#endif

/* EOF */
