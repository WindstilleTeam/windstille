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

#include "sprite3d/data.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include <biio/reader.hpp>

#include "app/app.hpp"
#include <wstdisplay/texture_manager.hpp>

namespace sprite3d {

static const int FORMAT_VERSION = 2;

Data::Data(std::filesystem::path const& filename) :
  meshs(),
  attachment_points(),
  actions()
{
  auto reader = biio::Reader::from_file(filename);

  try
  {
    std::string magic = reader.read_string(4);
    if(magic != "W3DS")
      throw std::runtime_error("Not a windstille 3d sprite file");
    uint16_t format_version = reader.read_le<uint16_t>();
    if(format_version > FORMAT_VERSION)
      throw std::runtime_error("sprite file format too new");
    if(format_version < FORMAT_VERSION)
      throw std::runtime_error("sprite file format too old");

    uint16_t mesh_count = reader.read_le<uint16_t>();
    if(mesh_count == 0)
      throw std::runtime_error("Sprite3D contains no meshs");
    uint16_t attachment_point_count = reader.read_le<uint16_t>();
    uint16_t action_count = reader.read_le<uint16_t>();
    if(action_count == 0)
      throw std::runtime_error("Sprite3D contains no actions");

    // read meshs
    meshs.resize(mesh_count);
    for(std::vector<Mesh>::iterator i = meshs.begin(); i != meshs.end(); ++i)
    {
      Mesh& mesh = *i;

      std::string texturename = reader.read_string0(64);
      mesh.triangle_count = reader.read_le<uint16_t>();
      mesh.vertex_count   = reader.read_le<uint16_t>();

      std::filesystem::path path = filename.parent_path();
      path /= std::filesystem::path(texturename).filename();
      mesh.texture = g_app.texture().get(path);

      // read triangles
      mesh.vertex_indices.reserve(mesh.triangle_count * 3);
      for(uint16_t v = 0; v < mesh.triangle_count * 3; ++v)
      {
        mesh.vertex_indices.push_back(reader.read_le<uint16_t>());
      }

      mesh.normals.reserve(mesh.triangle_count * 3);
      for(uint16_t n = 0; n < mesh.triangle_count * 3; ++n)
      {
        mesh.normals.push_back(reader.read_le<float>());
      }

      mesh.tex_coords.reserve(mesh.vertex_count * 2);
      for(uint16_t v = 0; v < mesh.vertex_count * 2; ++v)
      {
        mesh.tex_coords.push_back(reader.read_le<float>());
      }
    }

    // read attachment points
    attachment_points.reserve(attachment_point_count);
    for(uint16_t a = 0; a < attachment_point_count; ++a)
    {
      attachment_points.push_back(reader.read_string0(64));
    }

    // read actions
    actions.resize(action_count);
    for(std::vector<Action>::iterator i = actions.begin(); i != actions.end(); ++i)
    {
      Action& action = *i;

      action.name = reader.read_string0(64);
      action.speed = reader.read_le<float>();
      uint16_t marker_count = reader.read_le<uint16_t>();
      uint16_t frame_count  = reader.read_le<uint16_t>();

      // read markers
      action.markers.resize(marker_count);
      for(size_t m = 0; m < action.markers.size(); ++m)
      {
        Marker& marker = action.markers[m];
        marker.name  = reader.read_string0(64);
        marker.frame = reader.read_le<uint16_t>();
      }

      // read frames
      action.frames.resize(frame_count);
      for(size_t f = 0; f < action.frames.size(); ++f)
      {
        ActionFrame& frame = action.frames[f];

        frame.meshs.resize(mesh_count);
        for(uint16_t m = 0; m < mesh_count; ++m)
        {
          MeshVertices& mesh = frame.meshs[m];

          mesh.vertices.resize(meshs[m].vertex_count * 3);
          for(uint16_t v = 0; v < meshs[m].vertex_count * 3; ++v)
          {
            mesh.vertices[v] = reader.read_le<float>();
          }
        }

        frame.attachment_points.resize(attachment_point_count);
        for(uint16_t a = 0; a < attachment_point_count; ++a)
        {
          AttachmentPointPosition& point = frame.attachment_points[a];

          point.pos.x = reader.read_le<float>();
          point.pos.y = reader.read_le<float>();
          point.pos.z = reader.read_le<float>();

          point.quat.w = -reader.read_le<float>();
          point.quat.x =  reader.read_le<float>();
          point.quat.y =  reader.read_le<float>();
          point.quat.z =  reader.read_le<float>();
          point.quat = glm::normalize(point.quat);
        }
      }
    }
  }
  catch(std::exception& e)
  {
    std::ostringstream msg;
    msg << "Problem while reading '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }
}

Data::~Data()
{
}

const Action&
Data::get_action(const std::string& name) const
{
  for(std::vector<Action>::const_iterator action = actions.begin(); action != actions.end(); ++action) {
    if(action->name == name)
      return *action;
  }

  if (actions.empty())
  {
    std::ostringstream msg;
    msg << "No action with name '" << name << "' defined";
    throw std::runtime_error(msg.str());
  }
  else
  {
    std::cout << "No action with name '" << name << "' defined" << std::endl;
    return actions.front();
  }
}

const Marker&
Data::get_marker(const Action* action, const std::string& name) const
{
  for(size_t m = 0; m < action->markers.size(); ++m) {
    if(action->markers[m].name == name)
      return action->markers[m];
  }
  std::ostringstream msg;
  msg << "No marker with name '" << name << "' defined in action '"
      << action->name << "'";
  throw std::runtime_error(msg.str());
}

uint16_t
Data::get_attachment_point_id(const std::string& name) const
{
  for(size_t a = 0; a < attachment_points.size(); ++a) {
    if (attachment_points[a] == name) {
      return static_cast<uint16_t>(a);
    }
  }

  std::ostringstream msg;
  msg << "No Attachment Point with name '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

} // namespace sprite3d

/* EOF */

