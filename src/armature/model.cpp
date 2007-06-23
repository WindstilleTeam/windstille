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
#include "mesh.hpp"
#include "model.hpp"

Model::Model(FileReader& reader)
{
  if (reader.get_name() != "windstille-model")
    throw std::runtime_error("Not a 'windstille-model' file, its '" + reader.get_name() + "'");

  reader.get("name", name);

  std::vector<FileReader> sections = reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    {
      if (i->get_name() == "mesh")
        {
          Mesh* mesh = new Mesh(*i);
          meshes.push_back(mesh);
        }
      else
        {
          std::cout << "Ignoring unhandled tag: " << i->get_name() << std::endl;
        }
    }
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

/* EOF */
