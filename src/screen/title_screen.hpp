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

#ifndef HEADER_WINDSTILLE_SCREEN_TITLE_SCREEN_HPP
#define HEADER_WINDSTILLE_SCREEN_TITLE_SCREEN_HPP

#include <wstgui/screen.hpp>

#include "sprite2d/sprite.hpp"

namespace windstille {

class TitleScreen : public wstgui::Screen
{
private:
  Sprite background;

public:
  TitleScreen();
  ~TitleScreen() override;

  void on_startup() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;

  void handle_event(SDL_Event const& ) override;

private:
  TitleScreen (TitleScreen const&);
  TitleScreen& operator= (TitleScreen const&);
};

} // namespace windstille

#endif

/* EOF */
