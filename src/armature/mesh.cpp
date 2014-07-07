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

#include "armature/mesh.hpp"

#include <stdexcept>

#include "armature/armature.hpp"
#include "display/assert_gl.hpp"
#include "display/opengl_state.hpp"
#include "display/texture_manager.hpp"
#include "util/util.hpp"

Mesh::Mesh(FileReader& reader, const std::string& path) :
  name(),
  vertices(),
  normals(),
  texcoords(),
  triangles(),
  groups(),
  vertices_(),
  texture(),
  blend_sfactor(GL_ONE),
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
        {
          if (group.weight != 0.0f) // ignore useless bones
            groups.push_back(group);
        }
        else
        {
          std::cout << "Mesh::VertexGroup: Element missing" << std::endl;
        }
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
  
  // Add bone and weight to the individual vertices
  for(Groups::iterator i = groups.begin(); i != groups.end(); ++i)
  {
    VertexGroup& group = *i;
    for(std::vector<int>::iterator j = group.vertices.begin(); j != group.vertices.end(); ++j)
    {
      vertices_[*j].bone_names.push_back(group.bone_name);
      vertices_[*j].weights.push_back(group.weight);
    }
  }
  
  // Normalize Weight to 1.0f
  for(Vertices::iterator i = vertices_.begin(); i != vertices_.end(); ++i)
  {
    if (i->weights.empty())
      std::cout << "Vertex doesn't have weight: " << i - vertices_.begin() << std::endl;

    float total_weight = 0.0f;
    for(std::vector<float>::iterator w = i->weights.begin(); w != i->weights.end(); ++w)
      total_weight += *w;
     
    for(std::vector<float>::iterator w = i->weights.begin(); w != i->weights.end(); ++w)
      *w /= total_weight;
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

  texture_filename = path + basename(texture_filename);
  texture = TextureManager::current()->get(Pathname(texture_filename));

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

  for(Vertices::size_type i = 0; i < vertices_.size(); ++i)
  { // evil messing around with vertices, need more order
    vertices[3*i + 0] = vertices_[i].render_pos.x;
    vertices[3*i + 1] = vertices_[i].render_pos.y;
    vertices[3*i + 2] = vertices_[i].render_pos.z;
  }

  glVertexPointer(3, GL_FLOAT, 0, &*vertices.begin());

  glNormalPointer(GL_FLOAT, 0, &*normals.begin());
  glTexCoordPointer(2, GL_FLOAT, 0, &*texcoords.begin());
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(triangles.size()), GL_UNSIGNED_INT, &*triangles.begin());
}

void
Mesh::apply(Armature* armature)
{
  for(Vertices::iterator i = vertices_.begin(); i != vertices_.end(); ++i)
  {
    if (i->bone_names.empty())
    {
      // This shouldn't be reached for full mehes
      i->render_pos = i->pos;
    }
    else
    {
      if (i->bones.empty())
      {
        for(unsigned int j = 0; j < i->bone_names.size(); ++j)
          i->bones.push_back(armature->get_bone(i->bone_names[j]));
      }

      i->render_pos = Vector3(0.0f, 0.0f, 0.0f);
      for(unsigned int j = 0; j < i->bone_names.size(); ++j)
      {
        Bone* bone   = i->bones[j];
        float weight = i->weights[j];

        //std::cout << "apply: " << i->bone_names[j] << " " << bone << " " << weight << std::endl;

        if (bone)
        { // FIXME: Need to calculate the offset from the bone and rotate that, not the pos
          i->render_pos += glm::vec3((bone->render_matrix * glm::vec4(i->pos, 1.0f)) * weight);
        }
        else
        {
          std::cout << "Couldn't find bone: " << i->bone_names[j] << std::endl;
        }
      }
    }
  }
}

void
Mesh::reset()
{
  for(Vertices::iterator i = vertices_.begin(); i != vertices_.end(); ++i)
  {
    i->render_pos = i->pos;
  }
}

/* EOF */
