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
#include "display/display.hpp"
#include "math/vector3.hpp"
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
              Bone* bone = new Bone();
              i->get("name",     bone->name);
              i->get("children", bone->children_names);
              i->get("parent",   bone->parent_name);
              i->get("length",   bone->length);
              i->get("quat",     bone->quat);
              i->get("head",     bone->head);
              bones.push_back(bone);
            }
          else
            {
              std::cout << "Armature: unknown tag: " << i->get_name() << std::endl;
            }
        }
    }
  
  // insert code here to connet parent bones
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
    {
      Bone& bone = **i;
      for(std::vector<std::string>::iterator j = bone.children_names.begin(); j != bone.children_names.end(); ++j)
        {
          Bone* child = get_bone(*j);
          if (child)
            bone.children.push_back(child);
        }

      if (bone.parent_name.empty())
        {
          root_bone = &bone;
        }
      else
        {
          Bone* parent = get_bone(bone.parent_name);
          if (parent)
            bone.parent = parent;
        }
    }

  if (!root_bone)
    std::cout << "Root Bone Missing!" << std::endl;
}

Bone*
Armature::get_bone(const std::string& name)
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
    {
      if ((*i)->name == name)
        return *i;
    }
  std::cout << "Error: Bone: '" << name << "' not found" << std::endl;
  return 0;
}

void
Armature::draw()
{
  std::cout << "Draw" << std::endl;
  draw_bone(root_bone, Vector3(400,300, 0), Matrix::identity());
}

void
Armature::draw_bone(Bone* bone, Vector3 p, Matrix cur)
{
  // FIXME: not taking head into account
  Vector3 vec = Vector3(bone->length, 0.0f, 0.0f) + bone->head;
  Matrix  mat = cur.multiply(bone->quat.to_matrix());
  
  vec = p + mat.multiply(vec);  

  // std::cout << "vec: " << vec << std::endl;

  Display::draw_line(Vector(p.x,   p.y),
                     Vector(vec.x, vec.y),
                     Color(1.0f, 1.0f, 1.0f));

  for(std::vector<Bone*>::iterator i = bone->children.begin(); i != bone->children.end(); ++i)  
    {
      draw_bone(*i, vec, cur);
    }
}

/* EOF */
