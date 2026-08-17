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

#include "frame_hud.hpp"

#include <cmath>

#include <wstdisplay/graphics_context.hpp>

namespace wstgui {

FrameHud::FrameHud(Style& style) :
  m_style(style),
  m_time_counter(0),
  m_frame_counter(0),
  m_last_fps(0)
{
}

void
FrameHud::draw(wstdisplay::GraphicsContext& gc)
{
  m_frame_counter += 1;

  if (m_time_counter > 1)
  {
    m_last_fps = int(static_cast<float>(m_frame_counter) / m_time_counter);

    m_time_counter  = std::fmod(m_time_counter, 1.0f);
    m_frame_counter = 0;
  }

  if (m_last_fps != 0) {
    std::ostringstream out;
    out << "FPS: " << m_last_fps;
    m_style.get_font()->draw(gc, glm::vec2(static_cast<float>(gc.size().width()) - 100.0f, 30.0f), out.str());
  }
}

void
FrameHud::update(float delta, const Controller& controller)
{
  m_time_counter += delta;
}

} // namespace wstgui

/* EOF */
