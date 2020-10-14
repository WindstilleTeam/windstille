/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/decal_scale_control_point.hpp"

#include "display/scene_context.hpp"
#include "display/surface_manager.hpp"
#include "editor/app.hpp"
#include "util/pathname.hpp"

DecalScaleControlPoint::DecalScaleControlPoint(DecalObjectModel* object_, float ctrl_angle_, const glm::vec2& pos_,
                                               bool x_scale_, bool y_scale_) :
  ControlPoint(g_app.surface().get(Pathname("editor/scale_handle.png")), pos_),
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
DecalScaleControlPoint::on_move_update(GdkEventMotion* event, const glm::vec2& offset_)
{
  offset = offset_;

  glm::vec2 start   = pos - object->get_world_pos();
  glm::vec2 current = (pos + offset) - object->get_world_pos();

  start   = glm::rotate(start, -object->get_angle());
  current = glm::rotate(current, -object->get_angle());

  glm::vec2 new_scale = orig_scale;

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
DecalScaleControlPoint::on_move_end(GdkEventButton* /*event*/, const glm::vec2& offset_)
{
  on_move_update(nullptr/*event*/, offset_);
}

void
DecalScaleControlPoint::draw(SceneContext& sc)
{
  geom::frect rect = get_bounding_box();
  rect += offset;
  sc.control().draw_control(surface, pos, ctrl_angle);
}

/* EOF */
