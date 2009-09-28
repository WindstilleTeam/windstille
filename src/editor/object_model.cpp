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

#include "editor/object_model.hpp"

#include <iostream>
#include <sstream>

#include "display/scene_context.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "editor/constants.hpp"
#include "editor/editor_window.hpp"
#include "util/file_reader.hpp"

ObjectModel::ObjectModel(const std::string& name_, const Vector2f& rel_pos_)
  : name(name_),
    rel_pos(rel_pos_),
    select_mask(),
    parent_ptr()
{
}

ObjectModel::ObjectModel(const FileReader& reader)
  : name(),
    rel_pos(),
    select_mask(),
    parent_ptr()
{
  reader.get("name", name);
  reader.get("pos",  rel_pos);
  
  int mask = 1;
  reader.get("select-mask", mask);
  select_mask = SelectMask(static_cast<unsigned int>(mask));
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
    .write("select-mask", static_cast<int>(select_mask.get_mask()));
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
    return rel_pos + parent->get_world_pos();
  }
  else
  {
    return rel_pos;
  }
}

void
ObjectModel::set_world_pos(const Vector2f& p)
{
  if (parent_ptr.lock())
    set_rel_pos(rel_pos + (p - get_world_pos()));
  else
    set_rel_pos(p);
}

void
ObjectModel::set_rel_pos(const Vector2f& rel_pos_)
{
  // FIXME: Cut to integer positions, might not be the right place to do this
  rel_pos.x = floorf(rel_pos_.x);
  rel_pos.y = floorf(rel_pos_.y);
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
ObjectModel::reset()
{
}

bool
ObjectModel::is_at(const Vector2f& pos) const
{
  return get_bounding_box().is_inside(pos);
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

static float float_snap_to_grid(float v, float grid)
{
  return (roundf(v / grid) * grid) - v;
}

SnapData
ObjectModel::snap_to_grid(float grid_size) const
{
  const Rectf& r = get_bounding_box();

  Rectf snap_rect(float_snap_to_grid(r.left,   grid_size),
                  float_snap_to_grid(r.top,    grid_size),
                  float_snap_to_grid(r.right,  grid_size),
                  float_snap_to_grid(r.bottom, grid_size));

  SnapData best_snap;

  {
    SnapData snap;

    if (fabs(snap_rect.left) < g_snap_threshold)
    {
      snap.set_x(snap_rect.left);
    }

    if (fabs(snap_rect.top) < g_snap_threshold)
    {
      snap.set_y(snap_rect.top);
    }
      
    best_snap.merge(snap);
  }

  {
    SnapData snap;

    if (fabs(snap_rect.right) < g_snap_threshold)
    {
      snap.set_x(snap_rect.right);
    }

    if (fabs(snap_rect.bottom) < g_snap_threshold)
    {
      snap.set_y(snap_rect.bottom);
    }
      
    best_snap.merge(snap);
  }

  return best_snap;
}

SnapData
ObjectModel::snap_to_object(const Rectf& in) const
{
  const Rectf& rect = get_bounding_box();
  
  SnapData snap;

  float left_dist   = fabsf(rect.left   - in.right);
  float right_dist  = fabsf(rect.right  - in.left);
  float top_dist    = fabsf(rect.top    - in.bottom);
  float bottom_dist = fabsf(rect.bottom - in.top);
  float x_dist = std::min(left_dist, right_dist);
  float y_dist = std::min(top_dist, bottom_dist);

  if (x_dist < y_dist)
  { // closer on the X axis
    if (overlap(rect.top, rect.bottom,  in.top, in.bottom))
    {
      float y_snap = 0.0f;

      if (fabsf(rect.top - in.top) < g_snap_threshold)
      {
        y_snap = rect.top - in.top;
        snap.y_set = true;
      }

      if (fabsf(rect.bottom - in.bottom) < g_snap_threshold)
      {
        y_snap = rect.bottom - in.bottom;
        snap.y_set = true;
      }

      if (left_dist < right_dist)
      { // snap to left edge
        if (left_dist < g_snap_threshold)
        {
          snap.set_x(rect.left - in.right);
          snap.offset.y = y_snap;
        }
      }
      else
      { // snap to right edge
        if (right_dist < g_snap_threshold)
        {
          snap.set_x(rect.right - in.left);
          snap.offset.y = y_snap;
        }
      }
    }
  }
  else
  { // closer on the Y axis
    if (overlap(rect.left, rect.right,  in.left, in.right))
    {
      float x_snap = 0.0f;

      if (fabsf(rect.left - in.left) < g_snap_threshold)
      {
        x_snap = rect.left - in.left;
        snap.x_set = true;
      }

      if (fabsf(rect.right - in.right) < g_snap_threshold)
      {
        x_snap = rect.right - in.right;
        snap.x_set = true;
      }

      if (top_dist < bottom_dist)
      { // snap to top edge
        if (top_dist < g_snap_threshold)
        {
          snap.offset.x = x_snap;
          
          snap.set_y(rect.top - in.bottom);
        }
      }
      else
      { // snap to bottom edge
        if (bottom_dist < g_snap_threshold)
        {
          snap.offset.x = x_snap;
          snap.set_y(rect.bottom - in.top);
        }
      }
    }      
  }

  snap.offset = -snap.offset;

  return snap;
}

void
ObjectModel::get_property(TimelineProperty property, float& value_out) const
{
}

void
ObjectModel::get_property(TimelineProperty property, const Vector2f& value_out) const
{
  switch(property)
  {
    case kPosition:
      return rel_pos;
  }
}

void
ObjectModel::set_property(TimelineProperty property, float value)
{
}

void
ObjectModel::set_property(TimelineProperty property, const Vector2f& value)
{
  switch(property)
  {
    case kPosition:
      rel_pos = value;
      break;

    default:
      break;
  }
}

/* EOF */
