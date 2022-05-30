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

#include "armature/pose.hpp"

#include <iostream>
#include <stdexcept>

#include "util/file_reader.hpp"

namespace windstille {

Pose::Pose(ReaderDocument const& doc) :
  name(),
  bones()
{
  if (doc.get_name() != "pose") {
    throw std::runtime_error("not a pose file");
  } else {
    ReaderMapping const& reader = doc.get_mapping();
    reader.read("name",  name);

    ReaderCollection bones_collection;
    if (!reader.read("bones", bones_collection)) {
      std::cout << "Bones section missing" << std ::endl;
    } else {
      for(ReaderObject const& bone_obj : bones_collection.get_objects())
      {
        if (bone_obj.get_name() == "bone")
        {
          ReaderMapping const& bone_reader = bone_obj.get_mapping();

          PoseBone bone;
          bone_reader.read("name", bone.name);
          bone_reader.read("quat", bone.quat);
          bones.push_back(bone);
        }
        else
        {
          std::cout << "Unhandled tag: " << bone_obj.get_name() << std::endl;
        }
      }
    }
  }

  //std::cout << "Pose: " << name << " " << bones.size() << std::endl;
}

Pose::~Pose()
{
}

} // namespace windstille

/* EOF */
