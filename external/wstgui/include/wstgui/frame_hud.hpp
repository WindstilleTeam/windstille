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

#ifndef HEADER_WSTGUI_FRAME_HUD_HPP
#define HEADER_WSTGUI_FRAME_HUD_HPP

#include "screen.hpp"
#include "style.hpp"

namespace wstgui {

class FrameHud : public Screen
{
public:
  FrameHud(Style& style);

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

private:
  Style& m_style;

  float m_time_counter;
  int m_frame_counter;
  int m_last_fps;

private:
  FrameHud(const FrameHud&) = delete;
  FrameHud& operator=(const FrameHud&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
