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

#include "display/surface_drawing_parameters.hpp"
#include "display/scene_context.hpp"
#include "object_model.hpp"

ObjectModel::ObjectModel(const std::string& name_, const Vector2f& rel_pos_)
  : name(name_),
    rel_pos(rel_pos_)
{
}

ObjectModel::~ObjectModel()
{
}

void
ObjectModel::set_parent(const ObjectModelHandle& parent_)
{
  if (ObjectModelHandle parent = parent_ptr.lock())
    {
      rel_pos += parent->get_world_pos();
    }

  if (parent_.get())
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
ObjectModel::draw(SceneContext& sc)
{
  Vector2f wo_pos = get_world_pos();

  if (ObjectModelHandle parent = parent_ptr.lock())
    {
      sc.control().draw_line(wo_pos, parent->get_world_pos(), Color(0,0,1, 0.5f));
    }

  sc.control().fill_rect(Rectf(wo_pos - Vector2f(8, 8), Sizef(16, 16)), Color(1,0,0));
}

void
ObjectModel::on_move_start()
{  
}

void
ObjectModel::on_move_update(const Vector2f& offset)
{
  move_offset = offset;
}

void
ObjectModel::on_move_end(const Vector2f& offset)
{
  rel_pos += move_offset;
  move_offset = Vector2f(0,0);
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

bool
ObjectModel::snap_object(const Rectf& in, Vector2f& snap_offset_out) const
{
  const Rectf& rect = get_bounding_box();
  float snap_threshold = 32.0f;
  bool  snapped = false;

  // Reset offset to zero, since it might not be
  Vector2f snap_offset(0,0);

  float left_dist   = fabs(rect.left - in.right);
  float right_dist  = fabs(rect.right - in.left);
  float top_dist    = fabs(rect.top - in.bottom);
  float bottom_dist = fabs(rect.bottom - in.top);
  float x_dist = std::min(left_dist, right_dist);
  float y_dist = std::min(top_dist, bottom_dist);

  if (x_dist < y_dist)
    { // closer on the X axis
      if (overlap(rect.top, rect.bottom,
                  in.top, in.bottom))
        {
          if (left_dist < right_dist)
            { // snap to left edge
              if (left_dist < snap_threshold)
                {
                  snap_offset.x = rect.left - in.right;
                  snapped = true;
                
                }
            }
          else
            { // snap to right edge
              if (right_dist < snap_threshold)
                {
                  snap_offset.x = rect.right - in.left;
                  snapped = true;
                }
            }
        }
    }
  else
    { // closer on the Y axis
      if (overlap(rect.left, rect.right,
                  in.left, in.right))
        {
          if (top_dist < bottom_dist)
            { // snap to top edge
              if (top_dist < snap_threshold)
                {
                  snap_offset.y = rect.top - in.bottom;
                  snapped = true;
                }
            }
          else
            { // snap to bottom edge
              if (bottom_dist < snap_threshold)
                {
                  snap_offset.y = rect.bottom - in.top;
                  snapped = true;
                }
            }
        }      
    }

  if (snapped)
    snap_offset_out = snap_offset;

  return snapped;
}

/* EOF */
