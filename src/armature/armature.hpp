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

#ifndef HEADER_WINDSTILLE_ARMATURE_ARMATURE_HPP
#define HEADER_WINDSTILLE_ARMATURE_ARMATURE_HPP

#include "armature/bone.hpp"
#include "util/file_reader.hpp"

class Pose;

/** */
class Armature
{
private:
  std::string name;
  typedef std::vector<Bone*> Bones;
  Bones bones;

  Bone* root_bone;

public:
  Armature(FileReader& reader);
  ~Armature();

  /** Applies the given Pose */
  void apply(const Pose& pose);

  /** Sets the armature back into neutral position */
  void reset();

  void  parse(FileReader& reader);
  Bone* get_bone(const std::string& name);

  Matrix* get_render_matrix(const std::string& name);

  void  draw();
  void  draw_bone(Bone* bone, Vector3 p, Matrix matrix);
private:
  Armature (const Armature&);
  Armature& operator= (const Armature&);
};

#endif

/* EOF */
