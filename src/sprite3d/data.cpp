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

#include <boost/scoped_array.hpp>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include "util/util.hpp"
#include "display/texture_manager.hpp"

namespace sprite3d {

static const int FORMAT_VERSION = 2;

Data::Data(const Pathname& filename) :
  meshs(),
  attachment_points(),
  actions()
{
  std::ifstream in(filename.get_sys_path().c_str(), std::ios::binary);

  if (!in) 
  {
    std::ostringstream msg;
    msg << "Couldn't open '" << filename << "'";
    throw std::runtime_error(msg.str());
  }

  try 
  {
    std::string magic = read_string(in, 4);
    if(magic != "W3DS")
      throw std::runtime_error("Not a windstille 3d sprite file");
    uint16_t format_version = read_uint16_t(in);
    if(format_version > FORMAT_VERSION)
      throw std::runtime_error("sprite file format too new");
    if(format_version < FORMAT_VERSION)
      throw std::runtime_error("sprite file format too old");

    uint16_t mesh_count = read_uint16_t(in);
    if(mesh_count == 0)
      throw std::runtime_error("Sprite3D contains no meshs");
    uint16_t attachment_point_count = read_uint16_t(in);
    uint16_t action_count = read_uint16_t(in);
    if(action_count == 0)
      throw std::runtime_error("Sprite3D contains no actions");

    // read meshs
    meshs.resize(mesh_count);
    for(std::vector<Mesh>::iterator i = meshs.begin(); i != meshs.end(); ++i) 
    {
      Mesh& mesh = *i;

      std::string texturename = read_string(in, 64);
      mesh.triangle_count = read_uint16_t(in);
      mesh.vertex_count   = read_uint16_t(in);

      Pathname path = filename.get_dirname();
      path.append_path(basename(texturename));
      mesh.texture = TextureManager::current()->get(path);

      // read triangles
      mesh.vertex_indices.reserve(mesh.triangle_count * 3);
      for(uint16_t v = 0; v < mesh.triangle_count * 3; ++v) 
      {
        mesh.vertex_indices.push_back(read_uint16_t(in));
      }
      
      mesh.normals.reserve(mesh.triangle_count * 3);
      for(uint16_t n = 0; n < mesh.triangle_count * 3; ++n) 
      {
        mesh.normals.push_back(read_float(in));
      }

      mesh.tex_coords.reserve(mesh.vertex_count * 2);
      for(uint16_t v = 0; v < mesh.vertex_count * 2; ++v) 
      {
        mesh.tex_coords.push_back(read_float(in));
      }
    }

    // read attachment points
    attachment_points.reserve(attachment_point_count);
    for(uint16_t a = 0; a < attachment_point_count; ++a) 
    {
      attachment_points.push_back(read_string(in, 64));
    }

    // read actions
    actions.resize(action_count);
    for(std::vector<Action>::iterator i = actions.begin(); i != actions.end(); ++i) 
    {
      Action& action = *i;

      action.name = read_string(in, 64);
      action.speed = read_float(in);
      uint16_t marker_count = read_uint16_t(in);
      uint16_t frame_count  = read_uint16_t(in);

      // read markers
      action.markers.resize(marker_count);
      for(uint16_t m = 0; m < action.markers.size(); ++m) 
      {
        Marker& marker = action.markers[m];
        marker.name  = read_string(in, 64);
        marker.frame = read_uint16_t(in);
      }

      // read frames
      action.frames.resize(frame_count);
      for(uint16_t f = 0; f < action.frames.size(); ++f) 
      {
        ActionFrame& frame = action.frames[f];
        
        frame.meshs.resize(mesh_count);
        for(uint16_t m = 0; m < mesh_count; ++m) 
        {
          MeshVertices& mesh = frame.meshs[m];

          mesh.vertices.resize(meshs[m].vertex_count * 3);
          for(uint16_t v = 0; v < meshs[m].vertex_count * 3; ++v) 
          {
            mesh.vertices[v] = read_float(in);
          }
        }

        frame.attachment_points.resize(attachment_point_count);
        for(uint16_t a = 0; a < attachment_point_count; ++a) 
        {
          AttachmentPointPosition& point = frame.attachment_points[a];

          point.pos.x = read_float(in);
          point.pos.y = read_float(in);
          point.pos.z = read_float(in);

          point.quat.w = -read_float(in);
          point.quat.x =  read_float(in);
          point.quat.y =  read_float(in);
          point.quat.z =  read_float(in);
          point.quat.normalize();
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
  for(uint16_t m = 0; m < action->markers.size(); ++m) {
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
  for(uint16_t a = 0; a < attachment_points.size(); ++a) {
    if(attachment_points[a] == name)
      return a;
  }

  std::ostringstream msg;
  msg << "No Attachment Point with name '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

} // namespace sprite3d

/* EOF */

