/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/file_writer.hpp"

#include <wstdisplay/color.hpp>

namespace prio {

template<>
void write_custom(prio::Writer& writer, std::string_view key, glm::vec2 const& value)
{
  writer.write(key, std::vector<float>({value.x, value.y}));
}

template<>
void write_custom(prio::Writer& writer, std::string_view key, RGBAf const& value)
{
  writer.write(key, std::vector<float>({value.r, value.g, value.b, value.a}));
}

} // namespace prio

/* EOF */
