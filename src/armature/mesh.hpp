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

#ifndef HEADER_WINDSTILLE_ARMATURE_MESH_HPP
#define HEADER_WINDSTILLE_ARMATURE_MESH_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <wstdisplay/fwd.hpp>
#include <wstdisplay/texture.hpp>
#include "util/file_reader.hpp"

class Armature;
class Bone;

struct VertexGroup
{
  std::string bone_name;
  float       weight;
  std::vector<int> vertices;

  VertexGroup()
    : bone_name(),
      weight(),
      vertices()
  {}
};

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texcoord;

  glm::vec3 render_pos;

  // Influences from bone;
  std::vector<float> weights;
  std::vector<std::string> bone_names;
  std::vector<Bone*> bones;

  Vertex()
    : pos(),
      normal(),
      texcoord(),
      render_pos(),
      weights(),
      bone_names(),
      bones()
  {}
};

/** */
class Mesh
{
private:
  std::string name;

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<unsigned short> triangles;

  typedef std::vector<VertexGroup> Groups;
  Groups groups;

  typedef std::vector<Vertex> Vertices;
  Vertices vertices_;

  wstdisplay::TexturePtr texture;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

public:
  Mesh(ReaderMapping const& reader, std::filesystem::path const& basedir,
       wstdisplay::TextureManager& texture_manager);
  ~Mesh();

  void draw(wstdisplay::GraphicsContext& gc);
  void apply(Armature* armature);
  void reset();
private:
  Mesh (const Mesh&);
  Mesh& operator= (const Mesh&);
};

#endif

/* EOF */
