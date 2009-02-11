/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <stdexcept>
#include "util/file_reader.hpp"
#include "pose.hpp"

Pose::Pose(FileReader& reader)
{
  if (reader.get_name() != "pose")
    {
      throw std::runtime_error("not a pose file");
    }
  else
    {
      reader.get("name",  name);

      FileReader bones_reader;
      if (!reader.get("bones", bones_reader))
        {
          std::cout << "Bones section missing" << std::endl;
        }
      else
        {
          std::vector<FileReader> sections = bones_reader.get_sections();
          for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
            {
              if (i->get_name() == "bone")
                {
                  PoseBone bone;
                  i->get("name", bone.name);
                  i->get("quat", bone.quat);
                  bones.push_back(bone);
                }
              else
                {
                  std::cout << "Unhandled tag: " << i->get_name() << std::endl;
                }
            }
        }
    }

  //std::cout << "Pose: " << name << " " << bones.size() << std::endl;
}

/* EOF */
