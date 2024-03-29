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

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <wstdisplay/assert_gl.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>
#include "sprite3d/manager.hpp"
#include "sprite3d/sprite3d_drawable.hpp"

namespace windstille {

using namespace sprite3d;

Sprite3D::Sprite3D() :
  data(nullptr),
  actions_switched(false),
  frame1(),
  frame2(),
  blend_time(0.0f),
  next_frame(),
  next_action(),
  abort_at_frame(),
  blend_sfactor(GL_ONE),
  blend_dfactor(GL_ZERO)
{
}

Sprite3D::Sprite3D(std::filesystem::path const& filename, sprite3d::Manager& sprite3d_manager) :
  data(sprite3d_manager.create_data(filename)),
  actions_switched(false),
  frame1(),
  frame2(),
  blend_time(0.0f),
  next_frame(),
  next_action(),
  abort_at_frame(),
  blend_sfactor(GL_ONE),
  blend_dfactor(GL_ZERO)
{
  frame1.action         = &data->actions[0];
  frame1.frame          = 0;
  frame1.rot            = false;
  frame1.speed          = 1.0;
  frame2                = frame1;
  abort_at_frame.action = nullptr;
  next_frame.action     = nullptr;
  next_action.action    = nullptr;
}

Sprite3D::Sprite3D(Sprite3D const& rhs) :
  data(rhs.data),
  actions_switched(rhs.actions_switched),
  frame1(rhs.frame1),
  frame2(rhs.frame2),
  blend_time(rhs.blend_time),
  next_frame(rhs.next_frame),
  next_action(rhs.next_action),
  abort_at_frame(rhs.abort_at_frame),
  blend_sfactor(rhs.blend_sfactor),
  blend_dfactor(rhs.blend_dfactor)
{
}

Sprite3D&
Sprite3D::operator=(Sprite3D const& rhs)
{
  if (this != &rhs)
  {
    data             = rhs.data;
    actions_switched = rhs.actions_switched;
    frame1           = rhs.frame1;
    frame2           = rhs.frame2;
    blend_time       = rhs.blend_time;
    next_frame       = rhs.next_frame;
    next_action      = rhs.next_action;
    abort_at_frame   = rhs.abort_at_frame;
    blend_sfactor    = rhs.blend_sfactor;
    blend_dfactor    = rhs.blend_dfactor;
  }
  return *this;
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(std::string const& actionname, float speed)
{
  assert(data);
  next_frame.action = & data->get_action(actionname);
  // set to last action so that next set_next_frame call will result in frame 0
  if(speed >= 0) {
    next_frame.frame = 0;
  } else {
    next_frame.frame = static_cast<int>(next_frame.action->frames.size()) - 1;
  }
  next_frame.speed = speed;
  next_frame.rot = frame2.rot;

  next_action.action = nullptr;
  actions_switched = false;
}

std::string const&
Sprite3D::get_action() const
{
  if(next_frame.action != nullptr)
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
Sprite3D::set_next_action(std::string const& name, float speed)
{
  next_action.action = & data->get_action(name);
  if(speed >= 0) {
    next_action.frame = 0;
  } else {
    next_action.frame = static_cast<int>(next_action.action->frames.size()) - 1;
  }
  next_action.speed = speed;
  next_action.rot = frame2.rot;
  actions_switched = false;

  Frame const* frame = next_frame.action != nullptr ? &next_frame : &frame2;
  abort_at_frame.action = frame->action;
  abort_at_frame.speed = frame->speed;
  abort_at_frame.rot = frame->rot;
  if(frame->speed >= 0) {
    abort_at_frame.frame = static_cast<int>(frame->action->frames.size()) - 1;
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
Sprite3D::abort_at_marker(std::string const& name)
{
  Marker const& marker = data->get_marker(frame1.action, name);
  abort_at_frame = frame1;
  abort_at_frame.frame = marker.frame;
}

bool
Sprite3D::before_marker(std::string const& name) const
{
  Marker const& marker = data->get_marker(frame1.action, name);
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
  if ((speed <  0.0f && frame1.speed >= 0.0f) ||
      (speed >= 0.0f && frame1.speed <  0.0f))
  {
    blend_time = 1.0f - blend_time;
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
  if(next_frame.action != nullptr)
    return next_frame.rot;

  return frame1.rot;
}

Sprite3D::PointID
Sprite3D::get_attachment_point_id(std::string const& name) const
{
  return data->get_attachment_point_id(name);
}

glm::mat4
Sprite3D::get_attachment_point_matrix(PointID id) const
{
  AttachmentPointPosition const& point1
    = frame1.action->frames[frame1.frame].attachment_points[id];
  AttachmentPointPosition const& point2
    = frame2.action->frames[frame2.frame].attachment_points[id];

  glm::quat rotquat = glm::quat(0, 0, 1, 0);
  glm::quat quat1 = point1.quat;
  glm::vec3 pos1 = point1.pos;
  if(frame1.rot) {
    quat1 = rotquat * quat1;
    pos1.x = -pos1.x;
    pos1.z = -pos1.z;
  }
  glm::quat quat2 = point2.quat;
  glm::vec3 pos2 = point2.pos;
  if(frame2.rot) {
    quat2 = rotquat * quat2;
    pos2.x = -pos2.x;
    pos2.z = -pos2.z;
  }

  glm::vec3 pos = pos1 + (pos2 - pos1) * blend_time;
  glm::quat quat = glm::mix(quat1, quat2, blend_time);
  glm::mat4 result(1.0f);
  result = glm::translate(result, pos);
  result = result * glm::toMat4(quat);

  return result;
}

void
Sprite3D::set_next_frame()
{
  if(frame2.action != frame1.action && abort_at_frame.action == nullptr) {
    actions_switched = true;
  }

  frame1 = frame2;
  if(next_frame.action != nullptr) {
    frame2 = next_frame;
    next_frame.action = nullptr;
    return;
  }
  if(frame2 == abort_at_frame && next_action.action != nullptr) {
    frame2 = next_action;
    abort_at_frame.action = nullptr;
    next_action.action = nullptr;
    return;
  }

  frame2.action = frame1.action;
  if(frame1.speed < 0) {
    frame2.frame = (frame1.frame + static_cast<int>(frame1.action->frames.size()) - 1)
      % static_cast<int>(frame2.action->frames.size());
  } else {
    frame2.frame = (frame1.frame + 1) % static_cast<int>(frame1.action->frames.size());
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

  while(blend_time + time_delta >= 1.0f)
  {
    delta -= (1.0f - blend_time) / (frame1.action->speed * frame1.speed);
    set_next_frame();

    time_delta = delta * frame1.action->speed * frame1.speed;
    if(frame1.speed < 0)
      time_delta = -time_delta;
    blend_time = 0.0;
  }
  blend_time += time_delta;
}

void
Sprite3D::draw(wstdisplay::DrawingContext& dc, glm::vec2 const& pos, float z_pos)
{
  dc.draw(std::make_unique<Sprite3DDrawable>(*this, pos, z_pos, dc.get_modelview()));
}

static inline float interpolate(float v1, float v2, float t)
{
  return v1 + (v2 - v1) * t;
}

void
Sprite3D::draw(wstdisplay::GraphicsContext& gc, glm::vec2 const& pos, glm::mat4 const& modelview)
{
  gc.push_matrix();
  gc.mult_matrix(modelview);
  gc.translate(pos.x, pos.y, 0);
  if(frame1.rot) {
    gc.rotate(180, 0, 1.0, 0);
  }

  ActionFrame const& aframe1 = frame1.action->frames[frame1.frame];
  ActionFrame const& aframe2 = frame2.action->frames[frame2.frame];

  for(size_t m = 0; m < data->meshs.size(); ++m)
  {
    Mesh const& mesh = data->meshs[m];
    MeshVertices const& vertices1 = aframe1.meshs[m];
    MeshVertices const& vertices2 = aframe2.meshs[m];

    // blend between frame1 + frame2
    std::vector<float> verts(mesh.vertex_count * 3);
    if(frame1.rot == frame2.rot)
    {
      for(uint16_t v = 0; v < mesh.vertex_count*3; ++v)
      {
        float v1 = vertices1.vertices[v];
        float v2 = vertices2.vertices[v];
        verts[v] = interpolate(v1, v2, blend_time);
      }
    }
    else
    {
      // need to manually rotate 180 degree here because frames have different
      // rot values (=> x=-x, y=y, z=-z)
      for(uint16_t v = 0; v < mesh.vertex_count*3; )
      {
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

    wstdisplay::VertexArrayDrawable va;

    va.set_blend_func(blend_sfactor, blend_dfactor);
    va.set_depth_test(true);
    va.set_mode(GL_TRIANGLES);
    va.set_texture(mesh.texture);

    // FIXME: normals seem broken in .wsprite format, they are per
    // primitive, not per vertex, as they should be.
    // FIXME: va.add_normals(mesh.normals);
    va.add_texcoords(mesh.tex_coords);
    va.add_vertices(std::move(verts));
    va.add_indices(mesh.vertex_indices);

    va.render(gc);
  }

  assert_gl();

  gc.pop_matrix();
}

bool
Sprite3D::is_valid() const
{
  return data != nullptr;
}

void
Sprite3D::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

} // namespace windstille

/* EOF */
