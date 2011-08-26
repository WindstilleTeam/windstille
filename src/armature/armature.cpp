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

#include "armature/armature.hpp"

#include <GL/glew.h>
#include <stdexcept>

#include "display/opengl_state.hpp"
#include "display/display.hpp"
#include "display/color.hpp"
#include "armature/pose.hpp"

Armature::Armature(FileReader& reader) :
  name(),
  bones(),
  root_bone(0)
{
  parse(reader);
}

Armature::~Armature()
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
    delete *i;
  bones.clear();
}

void
Armature::parse(FileReader& reader)
{
  if (reader.get_name() != "armature")
  {
    throw std::runtime_error("Not an armature file: " + reader.get_name());
  }
  else
  {
    reader.get("name", name);
      
    FileReader bones_section;
    reader.get("bones", bones_section);

    std::vector<FileReader> bone_sections = bones_section.get_sections();
    for(std::vector<FileReader>::iterator i = bone_sections.begin(); i != bone_sections.end(); ++i)
    {
      if (i->get_name() == "bone")
      {
        std::auto_ptr<Bone> bone(new Bone());
        if (!(i->get("name",     bone->name) &&
              i->get("children", bone->children_names) &&
              i->get("parent",   bone->parent_name) &&
              i->get("length",   bone->length) &&
              i->get("quat",     bone->quat) &&
              i->get("head",     bone->offset)))
        {
          std::cout << "Error: some Bone attribute missing" << std::endl;
        }
        else
        {
          bone->render_matrix = bone->quat.to_matrix();
          bones.push_back(bone.release());
        }
      }
      else
      {
        std::cout << "Armature: unknown tag: " << i->get_name() << std::endl;
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
Armature::get_bone(const std::string& name_)
{
  for(Bones::iterator i = bones.begin(); i != bones.end(); ++i)
  {
    if ((*i)->name == name_)
      return *i;
  }
  std::cout << "Error: Bone: '" << name_ << "' not found" << std::endl;
  return 0;
}

void
Armature::draw()
{
  // For some reason OpenGL sometimes doesn't draw the lines properly,
  // going to NavigationTester and then back fixes the issue
  OpenGLState state;
  state.color(Color(1.0f, 0.0f, 0.0f));
  state.activate();

  glLineWidth(6.0f);
  //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX-" << std::endl;
  glBegin(GL_LINES);
  draw_bone(root_bone, Vector3(0,0, 0), Matrix::identity());
  glEnd();
}

void
Armature::draw_bone(Bone* bone, Vector3 p, Matrix m)
{
  Matrix  m_  = m.multiply(bone->render_matrix);
  Vector3 p_  = p + m.multiply(bone->offset);

  // FIXME: In theory we should be using length in the Z component, but only
  // Y seems to work?! -> Blenders matrix are weird
  Vector3 p__ = p_ + m_.multiply(Vector3(0.0f, bone->length, 0.0f));

  // p to p+offset
  glColor4f(0.0f, 0.5f, 0.0f, 0.5f);
  glVertex3f(  p.x, p.y, p.z);
  glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
  glVertex3f( p_.x, p_.y, p_.z);  

  // p+offset to new endpoint
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(  p_.x,   p_.y,   p_.z);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(  p__.x,  p__.y, p__.z);

  bone->render_head = p_;
  bone->render_tail = p__;

  for(std::vector<Bone*>::iterator i = bone->children.begin(); i != bone->children.end(); ++i)  
    draw_bone(*i, p__, m_);
}

void
Armature::apply(const Pose& pose)
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
        bone->render_matrix = bone->quat.to_matrix().multiply(pbone->quat.to_matrix());
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
    bone->render_matrix = bone->quat.to_matrix();
  }
}

/* EOF */
