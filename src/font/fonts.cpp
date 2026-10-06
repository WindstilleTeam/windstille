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

#include "font/fonts.hpp"

#include <wstdisplay/font/font.hpp>

#include "util/pathname.hpp"

namespace windstille {

Fonts::Fonts(wstdisplay::Device& device) :
  m_manager(device),
  ttffont(&m_manager.get(m_manager.load(Pathname("fonts/VeraMono.ttf").get_sys_path(), 14,
                                         wstdisplay::FontEffect{.border = 1}))),
  vera12(&m_manager.get(m_manager.load(Pathname("fonts/Vera.ttf").get_sys_path(), 12,
                                        wstdisplay::FontEffect{.border = 2}))),
  vera20(&m_manager.get(m_manager.load(Pathname("fonts/Vera.ttf").get_sys_path(), 20,
                                        wstdisplay::FontEffect{.border = 2})))
{
}

Fonts::~Fonts()
{
}

} // namespace windstille

/* EOF */
