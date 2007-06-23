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

#include <assert.h>
#include <stdexcept>
#include <iostream>
#include "util.hpp"
#include "display/opengl_state.hpp"
#include "display/texture_manager.hpp"
#include "file_reader.hpp"
#include "mesh.hpp"

Mesh::Mesh(FileReader& reader)
  : blend_sfactor(GL_ONE),
    blend_dfactor(GL_ZERO)
{
  if (reader.get_name() != "mesh")
    throw std::runtime_error("Not a 'mesh' type, its '" + reader.get_name() + "'");
  
  std::string texture_filename;

  reader.get("name",      name);
  reader.get("texture",   texture_filename);
  reader.get("vertices",  vertices);
  reader.get("normals",   normals);
  reader.get("texcoords", texcoords);
  reader.get("triangles", triangles);

  FileReader groups_reader;
  if (reader.get("groups", groups_reader))
    {
      std::vector<FileReader> sections = groups_reader.get_sections();
      for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
        {
          if ((*i).get_name() == "group")
            {
              VertexGroup group;
              if ((*i).get("bone",     group.bone_name) &&
                  (*i).get("weight",   group.weight) && 
                  (*i).get("vertices", group.vertices))
                groups.push_back(group);
              else
                std::cout << "Mesh::VertexGroup: Element missing" << std::endl;
            }
          else
            {
              std::cout << "Unknown tag: " << (*i).get_name() << std::endl;
            }
        }
    }

  // Check that all vectors have the same right modulo
  assert(vertices.size()  % 3 == 0);
  assert(normals.size()   % 3 == 0);
  assert(texcoords.size() % 2 == 0);

  // Convert the data to something we can use together with Armatures
  // and Bones
  for(std::vector<float>::size_type i = 0; i < vertices.size()/3; ++i)
    {
      Vertex vertex;

      vertex.pos.x = vertices[3*i+0];
      vertex.pos.y = vertices[3*i+1];
      vertex.pos.z = vertices[3*i+2];

      vertex.normal.x = normals[3*i+0];
      vertex.normal.y = normals[3*i+1];
      vertex.normal.z = normals[3*i+2];

      vertex.texcoord.x = texcoords[2*i+0];
      vertex.texcoord.y = texcoords[2*i+1];

      vertices_.push_back(vertex);
    }

  for(Groups::iterator i = groups.begin(); i != groups.end(); ++i)
    {
      VertexGroup& group = *i;
      for(std::vector<int>::iterator j = group.vertices.begin(); j != group.vertices.end(); ++j)
        {
          vertices_[*j].bone_names.push_back(group.bone_name);
          vertices_[*j].weight.push_back(group.weight);
        }
    }

#if 0 
  // FIXME: Broken by design
  FileReader influences_reader;
  if (reader.get("influences", influences_reader))
    {
      std::vector<FileReader> sections = influences_reader.get_sections();
      for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
        {
          if ((*i).get_name() == "vertex")
            {
              FileReader influences_sub_reader;

              (*i).get("index", index);
              if ((*i).get("influences", influences_sub_reader))
                {
                  std::vector<FileReader> sub_sections = influences_sub_reader.get_sections();
                  for(std::vector<FileReader>::iterator j = sub_sections.begin(); j != sub_sections.end(); ++j)
                    {
                      if ((*j).get_name() == "influences")
                        {
                          float weight;
                          std::string bone_name;
                      
                          (*j).get("weight", weight);
                          (*j).get("bone",   bone_name);                         
                        }
                    }
                }
            }
          else
            {
              std::cout << "Unknown tag: " << (*i).get_name() << std::endl;
            }
        }
    }
#endif

  texture = texture_manager->get(texture_filename);

  // Check that all vectors contain enough values for the given number
  // of vertices
  assert(vertices.size()/3 == normals.size()/3);
  assert(vertices.size()/3 == texcoords.size()/2);

  std::cout << "Number of normals:   " << normals.size()/3   << std::endl;
  std::cout << "Number of texcoords: " << texcoords.size()/2 << std::endl;
  std::cout << "Number of vertices:  " << vertices.size()/3  << std::endl;
  std::cout << "Number of triangles: " << triangles.size()/3 << std::endl;
}

Mesh::~Mesh()
{
  
}

void
Mesh::draw()
{
  //std::cout << "Mesh: Drawing: " << vertices.size() << std::endl;
  OpenGLState state;

  if (blend_sfactor != GL_ONE || blend_dfactor != GL_ZERO)
    {
      state.enable(GL_BLEND);
      state.set_blend_func(blend_sfactor, blend_dfactor);
    }
  else
    {
      state.enable(GL_DEPTH_TEST);
    }

  state.bind_texture(texture);

  glLineWidth(1.0f);
  glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
  state.enable_client_state(GL_VERTEX_ARRAY);
  state.enable_client_state(GL_NORMAL_ARRAY);
  state.enable_client_state(GL_TEXTURE_COORD_ARRAY);  
  state.activate();

  assert_gl("gl init before sprite");

  glVertexPointer(3, GL_FLOAT, 0, &*vertices.begin());
  glNormalPointer(GL_FLOAT, 0, &*normals.begin());
  glTexCoordPointer(2, GL_FLOAT, 0, &*texcoords.begin());
  glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, &*triangles.begin());
}

/* EOF */
