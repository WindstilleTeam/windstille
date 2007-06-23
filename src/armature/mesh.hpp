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

#ifndef HEADER_MESH_HPP
#define HEADER_MESH_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include "math/vector.hpp"
#include "math/vector3.hpp"
#include "display/texture.hpp"

class FileReader;
class Bone;

struct VertexGroup
{
  std::string bone_name;
  float       weight;
  std::vector<int> vertices;
};

struct Vertex
{
  Vector3 pos;
  Vector3 normal;
  Vector  texcoord;

  // Influences from bone;
  std::vector<float> weight;
  std::vector<std::string> bone_names;
  std::vector<Bone*> bones;
};

/** */
class Mesh
{
private:
  std::string name;

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<int>   triangles;

  typedef std::vector<VertexGroup> Groups;
  Groups groups;
  
  typedef std::vector<Vertex> Vertices;
  Vertices vertices_;

  Texture texture;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

public:
  Mesh(FileReader& reader, const std::string& path);
  ~Mesh();

  void draw();
private:
  Mesh (const Mesh&);
  Mesh& operator= (const Mesh&);
};

#endif

/* EOF */
