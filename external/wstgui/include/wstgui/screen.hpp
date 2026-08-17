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

#ifndef HEADER_WINDSTILLE_SCREEN_SCREEN_HPP
#define HEADER_WINDSTILLE_SCREEN_SCREEN_HPP

#include <SDL.h>
#include <wstdisplay/fwd.hpp>
#include <wstinput/fwd.hpp>

namespace wstgui {

using Controller = wstinput::Controller;

class Screen
{
public:
  Screen();
  virtual ~Screen();

  virtual void on_startup() {}

  /** Draw the current screen */
  virtual void draw(wstdisplay::GraphicsContext& gc) =0;

  /** Update the current screen by \a delta and with input from \a
      controller */
  virtual void update(float delta, const Controller& controller) =0;

  /** Called once a new SDL_Event arrives */
  virtual void handle_event(const SDL_Event& ) {}

  /** Marks the screen fro removal */
  virtual void finish() { m_finished = true; }
  virtual bool is_finished() { return m_finished; }

private:
  bool m_finished;
};

} // namespace wstgui

#endif

/* EOF */
