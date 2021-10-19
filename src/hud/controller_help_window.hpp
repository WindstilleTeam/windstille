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

#ifndef HEADER_WINDSTILLE_HUD_CONTROLLER_HELP_WINDOW_HPP
#define HEADER_WINDSTILLE_HUD_CONTROLLER_HELP_WINDOW_HPP

#include "screen/screen.hpp"

#include <glm/glm.hpp>
#include "util/currenton.hpp"

class ControllerHelpWindowImpl;

/** */
class ControllerHelpWindow : public Screen,
                             public Currenton<ControllerHelpWindow>
{
private:
  void draw_button(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, bool pressed);
  void draw_stick(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, bool pressed, float x, float y);
  void draw_trigger(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, float value);

public:
  ControllerHelpWindow();

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;
};

#endif

/* EOF */
