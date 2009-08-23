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
#include "display/scene_context.hpp"
#include "control_point.hpp"

ControlPointHandle
ControlPoint::create(const Vector2f& pos)
{
  return ControlPointHandle(new ControlPoint(Surface(Pathname("editor/handle.png")), pos));
}

ControlPoint::ControlPoint(const Surface& surface_, const Vector2f& pos_)
  : surface(surface_),
    pos(pos_)
{
}

ControlPoint::~ControlPoint()
{
}


void
ControlPoint::on_move_start(GdkEventButton* /*event*/)
{
}

void
ControlPoint::on_move_update(GdkEventMotion* /*event*/, const Vector2f& offset_)
{
  offset = offset_;
  std::cout << "on_move_update: " << offset << std::endl;
}

void
ControlPoint::on_move_end(GdkEventButton* /*event*/, const Vector2f& offset_)
{
  offset = offset_;
  offset = Vector2f();
}

Rectf
ControlPoint::get_bounding_box() const
{
  return Rectf(pos - Vector2f(11.0f, 11.0f), Sizef(22.0f, 22.0f));
}

void
ControlPoint::draw(SceneContext& sc)
{
  Rectf rect = get_bounding_box();

  rect += offset;

  //sc.control().fill_rect(rect.grow(4.0f), Color(0.0f, 0.0f, 0.0f));
  //sc.control().fill_rect(rect, Color(1.0f, 0.0f, 0.0f));
  sc.control().draw_control(surface, pos, 0.0f);
}

/* EOF */
