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

#ifndef HEADER_WINDSTILLE_ARMATURE_MODEL_HPP
#define HEADER_WINDSTILLE_ARMATURE_MODEL_HPP

#include <string>
#include <vector>

#include "util/file_reader.hpp"

class Mesh;
class Armature;

/** */
class Model
{
private:
  std::string name;
  typedef std::vector<Mesh*> Meshes;
  Meshes meshes;

public:
  Model(ReaderDocument const& doc, const std::string& path);
  ~Model();

  void draw();
  void apply(Armature* armature);
  void reset();
private:
  Model (const Model&);
  Model& operator= (const Model&);
};

#endif

/* EOF */
