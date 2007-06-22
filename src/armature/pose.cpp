/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <iostream>
#include <stdexcept>
#include "file_reader.hpp"
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
          std::cout << sections.size() << std::endl;
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

  std::cout << "Pose: " << name << " " << bones.size() << std::endl;
}

/* EOF */
