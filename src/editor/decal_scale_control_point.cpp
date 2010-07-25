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

#include "display/scene_context.hpp"
#include "editor/decal_scale_control_point.hpp"

DecalScaleControlPoint::DecalScaleControlPoint(DecalObjectModel* object_, float ctrl_angle_, const Vector2f& pos_, 
                                               bool x_scale_, bool y_scale_) :
  ControlPoint(Surface(Pathname("editor/scale_handle.png")), pos_),
  object(object_),
  ctrl_angle(ctrl_angle_),
  orig_scale(object_->get_scale()),
  x_scale(x_scale_),
  y_scale(y_scale_)
{}

void
DecalScaleControlPoint::on_move_start(GdkEventButton* event)
{ 
}

void
DecalScaleControlPoint::on_move_update(GdkEventMotion* event, const Vector2f& offset_) 
{
  offset = offset_; 

  Vector2f start   = pos - object->get_world_pos();
  Vector2f current = (pos + offset) - object->get_world_pos();

  start    = start.rotate(-object->get_angle());
  current = current.rotate(-object->get_angle());
    
  Vector2f new_scale = orig_scale;

  if (x_scale)
  {
    new_scale.x = current.x / start.x;
    new_scale.x *= orig_scale.x;
  }

  if (y_scale)
  {
    new_scale.y = current.y / start.y;
    new_scale.y *= orig_scale.y;
  }

  object->set_scale(new_scale);
}
  
void
DecalScaleControlPoint::on_move_end(GdkEventButton* /*event*/, const Vector2f& offset_)
{
  on_move_update(0/*event*/, offset_);
}

void
DecalScaleControlPoint::draw(SceneContext& sc)
{
  Rectf rect = get_bounding_box();
  rect += offset;
  sc.control().draw_control(surface, pos, ctrl_angle);
}

/* EOF */
