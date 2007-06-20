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
#include "armature.hpp"

Armature::Armature(FileReader& reader)
{
  parse(reader);
}

void
Armature::parse(FileReader& reader)
{
  if (reader.get_name() != "armature")
    {
      throw std::runtime_error("Not an armature file: " + reader.get_name());
    }
  else
    {
      reader.get("name", name);
      
      FileReader bones_section;
      reader.get("bones", bones_section);

      std::vector<FileReader> bone_sections = bones_section.get_sections();
      for(std::vector<FileReader>::iterator i = bone_sections.begin(); i != bone_sections.end(); ++i)
        {
          if (i->get_name() == "bone")
            {
              Bone bone;
              i->get("name",     bone.name);
              i->get("children", bone.children);
              i->get("length",   bone.length);
              i->get("quat",     bone.quat);
              i->get("head",     bone.head);
            }
          else
            {
              std::cout << "Armature: unknown tag: " << i->get_name() << std::endl;
            }
        }
    }
}

/* EOF */
