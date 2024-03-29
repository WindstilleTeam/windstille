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

#include "armature/armature.hpp"

#include <iostream>
#include <stdexcept>

#include <GL/glew.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <wstdisplay/graphics_context.hpp>
#include <surf/color.hpp>
#include "armature/pose.hpp"
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>

namespace windstille {

Armature::Armature(ReaderDocument const& doc) :
  name(),
  bones(),
  root_bone(nullptr)
{
  parse(doc);
}

Armature::~Armature()
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
    delete *i;
  bones.clear();
}

void
Armature::parse(ReaderDocument const& doc)
{
  if (doc.get_name() != "armature") {
    throw std::runtime_error("Not an armature file: " + doc.get_name());
  } else {
    ReaderMapping const& reader = doc.get_mapping();

    reader.read("name", name);

    ReaderCollection bones_collection;
    reader.read("bones", bones_collection);

    for (ReaderObject const& bone_obj : bones_collection.get_objects())
    {
      if (bone_obj.get_name() == "bone")
      {
        ReaderMapping const& bone_map = bone_obj.get_mapping();
        std::unique_ptr<Bone> bone(new Bone());

        bone_map.must_read("name",     bone->name);
        bone_map.must_read("children", bone->children_names);
        bone_map.read("parent",   bone->parent_name);
        bone_map.must_read("length",   bone->length);
        bone_map.must_read("quat",     bone->quat);
        bone_map.must_read("head",     bone->offset);

        bone->render_matrix = glm::toMat4(bone->quat);
        bones.push_back(bone.release());
      }
      else
      {
        std::cout << "Armature: unknown tag: " << bone_obj.get_name() << std::endl;
      }
    }
  }

  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
  {
    Bone& bone = **i;
    for(std::vector<std::string>::iterator j = bone.children_names.begin(); j != bone.children_names.end(); ++j)
    {
      Bone* child = get_bone(*j);
      if (child)
        bone.children.push_back(child);
    }

    if (bone.parent_name.empty())
    {
      root_bone = &bone;
    }
    else
    {
      Bone* parent = get_bone(bone.parent_name);
      if (parent)
        bone.parent = parent;
    }
  }

  if (!root_bone)
    std::cout << "Root Bone Missing!" << std::endl;
}

Bone*
Armature::get_bone(std::string const& name_)
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
  {
    if ((*i)->name == name_)
      return *i;
  }
  std::cout << "Error: Bone: '" << name_ << "' not found" << std::endl;
  return nullptr;
}

void
Armature::draw(wstdisplay::GraphicsContext& gc)
{
  // For some reason OpenGL sometimes doesn't draw the lines properly,
  // going to NavigationTester and then back fixes the issue
  glLineWidth(6.0f);

  wstdisplay::VertexArrayDrawable va;

  va.set_mode(GL_LINES);

  draw_bone(va, root_bone, glm::vec3(0,0, 0), glm::mat4(1.0f));

  va.render(gc);

  glLineWidth(1.0f);
}

void
Armature::draw_bone(wstdisplay::VertexArrayDrawable& va, Bone* bone, glm::vec3 p, glm::mat4 m)
{
  if (bone == nullptr) { return; }

  glm::mat4  m_  = m * bone->render_matrix;
  glm::vec3 p_  = glm::vec3(glm::vec4(p, 1.0f) + m * glm::vec4(bone->offset, 1.0f));

  // FIXME: In theory we should be using length in the Z component, but only
  // Y seems to work?! -> Blenders matrix are weird
  glm::vec3 p__ = glm::vec3(glm::vec4(p_, 1.0f) + m_ * glm::vec4(0.0f, bone->length, 0.0f, 1.0f));

  // p to p+offset
  va.color({0.0f, 0.5f, 0.0f, 0.5f});
  va.vertex(  p.x, p.y, p.z);

  va.color({0.0f, 1.0f, 0.0f, 0.5f});
  va.vertex( p_.x, p_.y, p_.z);

  // p+offset to new endpoint
  va.color({0.0f, 0.0f, 1.0f});
  va.vertex(  p_.x,   p_.y,   p_.z);

  va.color({1.0f, 0.0f, 0.0f});
  va.vertex(  p__.x,  p__.y, p__.z);

  bone->render_head = p_;
  bone->render_tail = p__;

  for(std::vector<Bone*>::iterator i = bone->children.begin(); i != bone->children.end(); ++i) {
    draw_bone(va, *i, p__, m_);
  }
}

void
Armature::apply(Pose const& pose)
{
  if (pose.get_name() != name)
  {
    std::cout << "Can't apply pose '" << pose.get_name() << "' to armature '" << name << "'" << std::endl;
  }
  else
  {
    for(std::vector<PoseBone>::const_iterator pbone = pose.bones.begin(); pbone != pose.bones.end(); ++pbone)
    {
      Bone* bone = get_bone(pbone->name);
      if (!bone)
      {
        std::cout << "Error: Couldn't find bone: " << pbone->name << std::endl;
      }
      else
      {
        bone->render_matrix = glm::mat4_cast(bone->quat) * glm::mat4_cast(pbone->quat);
      }
    }
  }
}

void
Armature::reset()
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
  {
    Bone* bone = *i;
    bone->render_matrix = glm::mat4_cast(bone->quat);
  }
}

} // namespace windstille

/* EOF */
