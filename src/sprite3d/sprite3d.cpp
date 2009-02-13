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

#include "sprite3d/sprite3d.hpp"

#include <assert.h>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <stdexcept>
#include <physfs.h>
#include "display/drawing_request.hpp"
#include "display/scene_context.hpp"
#include "app/globals.hpp"
#include "util/util.hpp"
#include "display/opengl_state.hpp"
#include "sprite3d/manager.hpp"
#include "sprite3d/data.hpp"

using namespace sprite3d;

Sprite3D::Sprite3D()
  : data(0), 
    actions_switched(false)
{
}

Sprite3D::Sprite3D(const std::string& filename)
  : data(sprite3d_manager->create_data(filename)),
    actions_switched(false)
{
  frame1.action         = &data->actions[0];
  frame1.frame          = 0;
  frame1.rot            = false;
  frame1.speed          = 1.0;
  frame2                = frame1;
  abort_at_frame.action = 0;
  next_frame.action     = 0;
  next_action.action    = 0;
  blend_time            = 0.0;

  blend_sfactor = GL_ONE;
  blend_dfactor = GL_ZERO;
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(const std::string& actionname, float speed)
{
  assert(data);
  next_frame.action = & data->get_action(actionname);
  // set to last action so that next set_next_frame call will result in frame 0
  if(speed >= 0) {
    next_frame.frame = 0;
  } else {
    next_frame.frame = next_frame.action->frames.size() - 1;
  }
  next_frame.speed = speed;
  next_frame.rot = frame2.rot;

  next_action.action = 0;
  actions_switched = false;
}

const std::string&
Sprite3D::get_action() const
{
  if(next_frame.action != 0)
    return next_frame.action->name;
  
  return frame2.action->name;
}

std::vector<std::string>
Sprite3D::get_actions() const
{
  std::vector<std::string> actions;
  for(std::vector<Action>::const_iterator i = data->actions.begin(); 
      i != data->actions.end(); 
      ++i)
    {
      actions.push_back(i->name);
    }
  return actions;
}

std::vector<std::string>
Sprite3D::get_attachment_points() const
{
  return data->attachment_points;
}

void
Sprite3D::set_next_action(const std::string& name, float speed)
{
  next_action.action = & data->get_action(name);
  if(speed >= 0) {
    next_action.frame = 0;
  } else {
    next_action.frame = next_action.action->frames.size() - 1;
  }
  next_action.speed = speed;
  next_action.rot = frame2.rot;
  actions_switched = false;

  const Frame* frame = next_frame.action != 0 ? &next_frame : &frame2;
  abort_at_frame.action = frame->action;
  abort_at_frame.speed = frame->speed;
  abort_at_frame.rot = frame->rot;
  if(frame->speed >= 0) {
    abort_at_frame.frame = frame->action->frames.size() - 1;
  } else {
    abort_at_frame.frame = 0;
  }
}

void
Sprite3D::set_next_rot(bool rot)
{
  next_action.rot = rot;
}

void
Sprite3D::abort_at_marker(const std::string& name)
{
  const Marker& marker = data->get_marker(frame1.action, name);
  abort_at_frame = frame1;
  abort_at_frame.frame = marker.frame;
}

bool
Sprite3D::before_marker(const std::string& name) const
{
  const Marker& marker = data->get_marker(frame1.action, name);  
  return frame1.frame < marker.frame;
}

bool
Sprite3D::switched_actions()
{
  if(actions_switched) {
    actions_switched = false;
    return true;
  }

  return false;
}

void
Sprite3D::set_speed(float speed)
{
  if((speed < 0 && frame1.speed >= 0) ||
     (speed >= 0 && frame1.speed < 0)) {
    blend_time = 1.0 - blend_time;
    std::swap(frame1, frame2);
  }
  frame1.speed = speed;
  frame2.speed = speed;
}

float
Sprite3D::get_speed() const
{
  return frame1.speed;
}

void
Sprite3D::set_rot(bool rot)
{
  next_frame.rot = rot;
}

bool
Sprite3D::get_rot() const
{
  if(next_frame.action != 0)
    return next_frame.rot;
  
  return frame1.rot;
}

Sprite3D::PointID
Sprite3D::get_attachment_point_id(const std::string& name) const
{
  return data->get_attachment_point_id(name); 
}

Matrix
Sprite3D::get_attachment_point_matrix(PointID id) const
{
  const AttachmentPointPosition& point1 
	  = frame1.action->frames[frame1.frame].attachment_points[id];
  const AttachmentPointPosition& point2 
	  = frame2.action->frames[frame2.frame].attachment_points[id];

  Quaternion rotquat = Quaternion(0, 0, 1, 0);
  Quaternion quat1 = point1.quat;
  Vector3 pos1 = point1.pos;
  if(frame1.rot) {
    quat1 = rotquat * quat1;
    pos1.x = -pos1.x;
    pos1.z = -pos1.z;
  }
  Quaternion quat2 = point2.quat;
  Vector3 pos2 = point2.pos;
  if(frame2.rot) {
    quat2 = rotquat * quat2;
    pos2.x = -pos2.x;
    pos2.z = -pos2.z;
  }

  Vector3 pos = pos1 + (pos2 - pos1) * blend_time;
  Quaternion quat = quat1.slerp(quat2, blend_time);
  Matrix result = pos.to_matrix();
  result = result.multiply(quat.to_matrix());

  return result;
}

class Sprite3DDrawingRequest : public DrawingRequest
{
private:
  const Sprite3D* sprite;

public:
  Sprite3DDrawingRequest(const Sprite3D* sprite, const Vector2f& pos, float z_pos,
                       const Matrix& modelview)
    : DrawingRequest(pos, z_pos, modelview), sprite(sprite)
  {}

  void draw(const Texture& tmp_texture)
  {
    sprite->draw(pos, modelview);
  }
};

void
Sprite3D::set_next_frame()
{
  if(frame2.action != frame1.action && abort_at_frame.action == 0) {
    actions_switched = true;
  }
  
  frame1 = frame2;
  if(next_frame.action != 0) {
    frame2 = next_frame;
    next_frame.action = 0;
    return;
  }
  if(frame2 == abort_at_frame && next_action.action != 0) {
    frame2 = next_action;
    abort_at_frame.action = 0;
    next_action.action = 0;
    return;
  }

  frame2.action = frame1.action;
  if(frame1.speed < 0) {
    frame2.frame = (frame1.frame + frame1.action->frames.size() - 1)
      % frame2.action->frames.size();
  } else {
    frame2.frame = (frame1.frame + 1) % frame1.action->frames.size();
  }
  frame2.speed = frame1.speed;
  frame2.rot = frame1.rot;
}

void
Sprite3D::update(float delta)
{   
  float time_delta = delta * frame1.action->speed * frame1.speed;
  if(frame1.speed < 0)
    time_delta = -time_delta;
  
  while(blend_time + time_delta >= 1.0) {
    delta -= (1.0 - blend_time) / (frame1.action->speed * frame1.speed);
    set_next_frame();

    time_delta = delta * frame1.action->speed * frame1.speed;
    if(frame1.speed < 0)
      time_delta = -time_delta;
    blend_time = 0.0;
  }
  blend_time += time_delta;
}

void
Sprite3D::draw(DrawingContext& dc, const Vector2f& pos, float z_pos) const
{
  dc.draw(new Sprite3DDrawingRequest(this, pos, z_pos, dc.get_modelview()));
}

void
Sprite3D::draw(DrawingContext& dc, const Matrix& , float ) const
{
  dc.draw(new Sprite3DDrawingRequest(this, Vector2f(0, 0), 0.0f, dc.get_modelview()));
}

static inline float interpolate(float v1, float v2, float t)
{
  return v1 + (v2 - v1) * t;
}

void
Sprite3D::draw(const Vector2f& pos, const Matrix& modelview) const
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); 
  glMultMatrixf(modelview.matrix);
  glTranslatef(pos.x, pos.y, 0);
  if(frame1.rot) {
    glRotatef(180, 0, 1.0, 0);
  } 

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

  state.enable_client_state(GL_VERTEX_ARRAY);
  state.enable_client_state(GL_NORMAL_ARRAY);
  state.enable_client_state(GL_TEXTURE_COORD_ARRAY);  

  assert_gl("gl init before sprite");

  const ActionFrame& aframe1 = frame1.action->frames[frame1.frame];
  const ActionFrame& aframe2 = frame2.action->frames[frame2.frame];
  
  for(uint16_t m = 0; m < data->meshs.size(); ++m) 
    {
      const Mesh& mesh = data->meshs[m];
      const MeshVertices& vertices1 = aframe1.meshs[m];
      const MeshVertices& vertices2 = aframe2.meshs[m];

      // set texture
      state.bind_texture(mesh.texture);

      // blend between frame1 + frame2
      float* verts = new float[mesh.vertex_count * 3];
      if(frame1.rot == frame2.rot) {
        for(uint16_t v = 0; v < mesh.vertex_count*3; ++v) {
          float v1 = vertices1.vertices[v];
          float v2 = vertices2.vertices[v];
          verts[v] = interpolate(v1, v2, blend_time);
        }
      } else {
        // need to manually rotate 180 degree here because frames have different
        // rot values (=> x=-x, y=y, z=-z)
        for(uint16_t v = 0; v < mesh.vertex_count*3; ) {
          // X coord
          float v1 = vertices1.vertices[v];
          float v2 = -vertices2.vertices[v];
          verts[v++] = interpolate(v1, v2, blend_time);
          // Y coord
          v1 = vertices1.vertices[v];
          v2 = vertices2.vertices[v];
          verts[v++] = interpolate(v1, v2, blend_time);
          // Z coord
          v1 = vertices1.vertices[v];
          v2 = -vertices2.vertices[v];                            
          verts[v++] = interpolate(v1, v2, blend_time);          
        }
      }

      state.activate();

      // draw mesh
      glVertexPointer(3, GL_FLOAT, 0, verts);
      glNormalPointer(GL_FLOAT, 0, &*mesh.normals.begin());
      glTexCoordPointer(2, GL_FLOAT, 0, &*mesh.tex_coords.begin());
      glDrawElements(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT,
                     &*mesh.vertex_indices.begin());

      delete[] verts;
    }

  assert_gl("rendering 3d sprite");      

  glPopMatrix();
}

bool
Sprite3D::is_valid() const
{
  return data != 0;
}

void
Sprite3D::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

/* EOF */
