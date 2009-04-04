/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <sstream>

#include "editor_window.hpp"
#include "util/file_reader.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/scene_context.hpp"
#include "object_model.hpp"

ObjectModel::ObjectModel(const std::string& name_, const Vector2f& rel_pos_)
  : name(name_),
    rel_pos(rel_pos_)
{
}

ObjectModel::ObjectModel(const FileReader& reader)
{
  reader.get("name", name);
  reader.get("pos",  rel_pos);
  
  int mask = 1;
  reader.get("select-mask", mask);
  select_mask = SelectMask(mask);
}

ObjectModel::~ObjectModel()
{
}

std::string
ObjectModel::get_id() const
{
  std::ostringstream os;
  os << this;
  return os.str();
}

FileWriter&
ObjectModel::write_member(FileWriter& writer) const
{
  ObjectModelHandle parent = parent_ptr.lock();

  return writer
    .write("id", get_id())
    .write("pos", rel_pos)
    .write("parent", parent.get() ? parent->get_id() : "")
    .write("select-mask", select_mask.get_mask());
}

ObjectModelHandle
ObjectModel::get_parent() const
{
  ObjectModelHandle parent = parent_ptr.lock();
  return parent;
}

void
ObjectModel::set_parent(const ObjectModelHandle& parent_, bool recalc_pos)
{
  { // Remove the old parent
    ObjectModelHandle parent = parent_ptr.lock();
    if (parent)
      { 
        rel_pos += parent->get_world_pos();
        parent_ptr = ObjectModelPtr();
      }
  }

  // Check that we don't create a loop to 'this' by parenting
  ObjectModelHandle pptr = parent_;
  while(pptr.get())
    {
      if (pptr.get() == this)
        {
          EditorWindow::current()->print("Error: Trying to create parent loop");

          parent_ptr = ObjectModelPtr();
          return;
        }
      pptr = pptr->get_parent();
    }

  // Set new parent
  if (parent_.get() && recalc_pos)
    {
      rel_pos -= parent_->get_world_pos();
    }

  parent_ptr = parent_;
}

Vector2f
ObjectModel::get_world_pos() const
{
  ObjectModelHandle parent = parent_ptr.lock();
  if (parent.get())
    {
      return rel_pos + move_offset + parent->get_world_pos();
    }
  else
    {
      return rel_pos + move_offset;
    }
}

void
ObjectModel::set_world_pos(const Vector2f& p)
{
  if (parent_ptr.lock())
    rel_pos += p - get_world_pos();
  else
    rel_pos = p;
}

void
ObjectModel::draw_select(SceneContext& sc, bool highlight)
{
  if (highlight)
    sc.control().draw_rect(get_bounding_box(), Color(1.0f, 1.0f, 1.0f, 1.0f));
  else
    sc.control().draw_rect(get_bounding_box(), Color(0.5f, 0.5f, 1.0f, 1.0f));
}

void
ObjectModel::draw(SceneContext& sc)
{
  Vector2f wo_pos = get_world_pos();

  if (ObjectModelHandle parent = parent_ptr.lock())
    {
      sc.control().draw_line(wo_pos, parent->get_world_pos(), Color(0,0,1, 0.5f));
    }

  //sc.control().fill_rect(Rectf(wo_pos - Vector2f(8, 8), Sizef(16, 16)), Color(1,0,0));
}

void
ObjectModel::on_move_start()
{
}

void
ObjectModel::on_move_update(const Vector2f& offset)
{
  // Cut to integer positions
  move_offset.x = floorf(offset.x);
  move_offset.y = floorf(offset.y);

  // Cut to integer positions
  rel_pos.x = floorf(rel_pos.x);
  rel_pos.y = floorf(rel_pos.y);
}

void
ObjectModel::on_move_end(const Vector2f& offset)
{
  // Cut to integer positions
  move_offset.x = floorf(offset.x);
  move_offset.y = floorf(offset.y);

  rel_pos += move_offset;
  move_offset = Vector2f(0,0);

  // Cut to integer positions
  rel_pos.x = floorf(rel_pos.x);
  rel_pos.y = floorf(rel_pos.y);
}

void
ObjectModel::reset()
{
}

static bool overlap(float l1, float r1,
                    float l2, float r2)
{
  if (r1 < l2)
    return false;
  else if (r2 < l1)
    return false;
  else
    return true;
}

SnapData
ObjectModel::snap_object(const Rectf& in) const
{
  const Rectf& rect = get_bounding_box();
  float snap_threshold = 16.0f;
  
  // Reset offset to zero, since it might not be
  SnapData snap;

  float left_dist   = fabs(rect.left - in.right);
  float right_dist  = fabs(rect.right - in.left);
  float top_dist    = fabs(rect.top - in.bottom);
  float bottom_dist = fabs(rect.bottom - in.top);
  float x_dist = std::min(left_dist, right_dist);
  float y_dist = std::min(top_dist, bottom_dist);

  if (x_dist < y_dist)
    { // closer on the X axis
      if (overlap(rect.top, rect.bottom,  in.top, in.bottom))
        {
          float y_snap = 0.0f;

          if (fabs(rect.top - in.top) < snap_threshold)
            {
              y_snap = rect.top - in.top;
              snap.y_set = true;
            }

          if (fabs(rect.bottom - in.bottom) < snap_threshold)
            {
              y_snap = rect.bottom - in.bottom;
              snap.y_set = true;
            }

          if (left_dist < right_dist)
            { // snap to left edge
              if (left_dist < snap_threshold)
                {
                  snap.offset.x = rect.left - in.right;
                  snap.offset.y = y_snap;
                  snap.x_set = true;
                }
            }
          else
            { // snap to right edge
              if (right_dist < snap_threshold)
                {
                  snap.offset.x = rect.right - in.left;
                  snap.offset.y = y_snap;
                  snap.x_set = true;
                }
            }
        }
    }
  else
    { // closer on the Y axis
      if (overlap(rect.left, rect.right,  in.left, in.right))
        {
          float x_snap = 0.0f;

          if (fabs(rect.left - in.left) < snap_threshold)
            {
              x_snap = rect.left - in.left;
              snap.x_set = true;
            }

          if (fabs(rect.right - in.right) < snap_threshold)
            {
              x_snap = rect.right - in.right;
              snap.x_set = true;
            }

          if (top_dist < bottom_dist)
            { // snap to top edge
              if (top_dist < snap_threshold)
                {
                  snap.offset.x = x_snap;
                  snap.offset.y = rect.top - in.bottom;
                  snap.y_set = true;
                }
            }
          else
            { // snap to bottom edge
              if (bottom_dist < snap_threshold)
                {
                  snap.offset.x = x_snap;
                  snap.offset.y = rect.bottom - in.top;
                  snap.y_set = true;
                }
            }
        }      
    }

  return snap;
}

/* EOF */
