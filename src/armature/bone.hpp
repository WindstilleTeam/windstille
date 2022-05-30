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

#ifndef HEADER_WINDSTILLE_ARMATURE_BONE_HPP
#define HEADER_WINDSTILLE_ARMATURE_BONE_HPP

#include <string>
#include <vector>

#include <glm/gtc/quaternion.hpp>

class Bone
{
private:
public:
  std::string name;

  std::vector<std::string> children_names;
  std::string parent_name;

  std::vector<Bone*> children;
  Bone*      parent;

  float      length;

  glm::quat quat;
  glm::vec3    offset;

  glm::mat4     render_matrix;
  glm::vec3    render_head;
  glm::vec3    render_tail;

  Bone();
  ~Bone();

private:
  Bone (Bone const&);
  Bone& operator= (Bone const&);
};

#endif

/* EOF */
