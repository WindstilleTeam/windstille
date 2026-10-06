/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_TRANSFORM_HPP
#define HEADER_WINDSTILLE_DISPLAY_TRANSFORM_HPP

#include <glm/glm.hpp>

namespace windstille {

/** The 2D part of a 3D transform, for placing 2D graphics at e.g. an
    attachment point of a Sprite3D: x and y of the transformed axes
    and of the translation, z is dropped. */
inline glm::mat3 flatten(glm::mat4 const& m)
{
  return glm::mat3(glm::vec3(m[0].x, m[0].y, 0.0f),
                   glm::vec3(m[1].x, m[1].y, 0.0f),
                   glm::vec3(m[3].x, m[3].y, 1.0f));
}

} // namespace windstille

#endif

/* EOF */
