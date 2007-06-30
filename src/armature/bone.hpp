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

#ifndef HEADER_SKELETON_BONE_HPP
#define HEADER_SKELETON_BONE_HPP

#include <string>
#include <vector>
#include "math/quaternion.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"

/** */
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

  Quaternion quat;
  Vector3    offset;

  Matrix     render_matrix;
  Vector3    render_head;
  Vector3    render_tail;
  
  Bone();
  ~Bone();

private:
  Bone (const Bone&);
  Bone& operator= (const Bone&);
};

#endif

/* EOF */
