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

#include <stdexcept>
#include "util/file_reader.hpp"
#include "armature/mesh.hpp"
#include "armature/model.hpp"

Model::Model(FileReader& reader, const std::string& path)
  : name(),
    meshes()
{
  if (reader.get_name() != "windstille-model")
    throw std::runtime_error("Not a 'windstille-model' file, its '" + reader.get_name() + "'");

  reader.get("name", name);

  std::vector<FileReader> sections = reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    {
      if (i->get_name() == "mesh")
        {
          Mesh* mesh = new Mesh(*i, path);
          meshes.push_back(mesh);
        }
      else
        {
          std::cout << "Ignoring unhandled tag: " << i->get_name() << std::endl;
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
