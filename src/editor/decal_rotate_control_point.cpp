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
#include "editor/decal_rotate_control_point.hpp"

DecalRotateControlPoint::DecalRotateControlPoint(DecalObjectModel* object_, float ctrl_angle_, const Vector2f& pos_)
  : ControlPoint(Surface(Pathname("editor/rotate_handle.png")), pos_),
    object(object_),
    ctrl_angle(ctrl_angle_),
    orig_angle(object->get_angle()),
    center(object->get_world_pos())
{
}

void
DecalRotateControlPoint::on_move_start(GdkEventMotion* event) 
{
}

void
DecalRotateControlPoint::on_move_update(GdkEventMotion* event, const Vector2f& offset_) 
{
  offset = offset_;

  Vector2f base = pos - center;
  float base_angle = atan2f(base.y, base.x);

  Vector2f current = (pos+offset) - center;
  float current_angle = atan2f(current.y, current.x);

  float new_angle = orig_angle + current_angle - base_angle;

  if (event->state & GDK_CONTROL_MASK)
  {
    float steps = 16.0f;
    new_angle = roundf((new_angle / (2.0f * math::pi)) * steps) / steps * 2.0f * math::pi;
  }

  object->set_angle(new_angle);
}
  
void
DecalRotateControlPoint::on_move_end(GdkEventMotion* event, const Vector2f& offset_)
{
  on_move_update(event, offset_);
}

void
DecalRotateControlPoint::draw(SceneContext& sc)
{
  Rectf rect = get_bounding_box();
  rect += offset;
  sc.control().draw_control(surface, pos, ctrl_angle);
}

/* EOF */
