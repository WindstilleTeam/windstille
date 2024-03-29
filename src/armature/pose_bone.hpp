/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_ARMATURE_POSE_BONE_HPP
#define HEADER_WINDSTILLE_ARMATURE_POSE_BONE_HPP

#include <string>

#include <glm/gtc/quaternion.hpp>

namespace windstille {

class PoseBone
{
public:
  std::string name;
  int         bone_id;
  glm::quat  quat;

  PoseBone()
    : name(),
      bone_id(),
      quat()
  {}
};

} // namespace windstille

#endif

/* EOF */
