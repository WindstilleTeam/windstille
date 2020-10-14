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

#include "armature/model.hpp"

#include <iostream>
#include <stdexcept>

#include "util/file_reader.hpp"
#include "armature/mesh.hpp"

Model::Model(ReaderDocument const& doc, std::filesystem::path const& basedir,
             TextureManager& texture_manager) :
  name(),
  meshes()
{
  if (doc.get_name() != "windstille-model") {
    throw std::runtime_error("Not a 'windstille-model' file, its '" + doc.get_name() + "'");
  }

  ReaderMapping const& reader = doc.get_mapping();

  reader.read("name", name);

  ReaderCollection mesh_collection;
  reader.read("meshes", mesh_collection);
  for (ReaderObject const& mesh_obj : mesh_collection.get_objects())
  {
    if (mesh_obj.get_name() == "mesh") {
      Mesh* mesh = new Mesh(mesh_obj.get_mapping(), basedir, texture_manager);
      meshes.push_back(mesh);
    } else {
      std::cout << "Ignoring unhandled tag: " << mesh_obj.get_name() << std::endl;
    }
  }

  reset();
}

Model::~Model()
{
  for(Meshes::iterator i = meshes.begin(); i != meshes.end(); ++i)
    delete *i;
}

void
Model::draw()
{
  for(Meshes::iterator i = meshes.begin(); i != meshes.end(); ++i)
  {
    (*i)->draw();
  }
}

void
Model::reset()
{
  for(Meshes::iterator i = meshes.begin(); i != meshes.end(); ++i)
  {
    (*i)->reset();
  }
}

void
Model::apply(Armature* armature)
{
  for(Meshes::iterator i = meshes.begin(); i != meshes.end(); ++i)
  {
    (*i)->apply(armature);
  }
}

/* EOF */
