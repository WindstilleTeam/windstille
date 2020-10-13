/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_SPRITE3D_DATA_HPP
#define HEADER_WINDSTILLE_SPRITE3D_DATA_HPP

#include <GL/glew.h>
#include <vector>
#include <stdint.h>

#include "display/texture.hpp"
#include "math/quaternion.hpp"
#include "math/vector3.hpp"

namespace sprite3d {

struct Mesh;
struct AttachmentPoint;
struct MeshVertices;
struct ActionFrame;
struct Action;
struct Marker;

/**
 * This class holds the data of a .wsprite file, ie. all data that is
 * needed to create a Sprite3D.
 */
class Data
{
public:
  Data(std::filesystem::path const& filename);
  ~Data();

  const Action& get_action(const std::string& name) const;
  const Marker& get_marker(const Action* action, const std::string& name) const;
  uint16_t      get_attachment_point_id(const std::string& name) const;

  std::vector<Mesh>        meshs;
  std::vector<std::string> attachment_points;
  std::vector<Action>      actions;

private:
  Data (const Data&);
  Data& operator= (const Data&);
};

/**
 * A simple Mesh, consisting of a texture and triangles, each triangle
 * consists of vertex indices (not positions, those are stored in the
 * Action), textured coordinates and normals
 */
struct Mesh
{
  TexturePtr   texture;

  uint16_t  vertex_count;
  uint16_t  triangle_count;

  // Triangle Data
  std::vector<uint16_t> vertex_indices;
  std::vector<float>    tex_coords;
  std::vector<float>    normals;

  Mesh()
    : texture(),
      vertex_count(),
      triangle_count(),
      vertex_indices(),
      tex_coords(),
      normals()
  {}
};

/** Position of an AttachmentPoint */
struct AttachmentPointPosition
{
  Vector3    pos;  // x, y, z
  Quaternion quat; // w, x, y, z

  AttachmentPointPosition()
    : pos(),
      quat()
  {}
};

/**
 * MeshVertices holds the positions of vertices for a mesh.
 */
struct MeshVertices
{
  MeshVertices() : vertices() {}
  std::vector<float> vertices;
};

/**
 * A Marker is a named-frame, so that you can refer to a frame number
 * by its purpose, ie. 'RightFoot', to refer to the frame where the
 * right foot is on the ground. This is used to ensure that the
 * transitions from one animation to the next are smooth.
 */
struct Marker
{
  std::string name;
  uint16_t    frame;

  Marker()
    : name(),
      frame()
  {}
};

/**
 * A ActionFrame holds the vertex positions for the mesh and the
 * position of the attachment points
 */
struct ActionFrame
{
  ActionFrame()
    : meshs(),
      attachment_points()
  { }
  std::vector<MeshVertices> meshs;
  std::vector<AttachmentPointPosition> attachment_points;
};

/**
 *
 */
struct Action
{
  Action()
    : name(),
      speed(),
      markers(0),
      frames(0)
  { }
  std::string  name;
  float        speed;
  std::vector<Marker> markers;
  std::vector<ActionFrame> frames;
};

} // namespace sprite3d

#endif

/* EOF */
