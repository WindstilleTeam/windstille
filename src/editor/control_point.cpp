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
  return ControlPointHandle(new ControlPoint(pos));
}

ControlPoint::ControlPoint(const Vector2f& pos_)
  : pos(pos_)
{
}

ControlPoint::~ControlPoint()
{
}


void
ControlPoint::on_move_start()
{
}

void
ControlPoint::on_move_update(const Vector2f& offset_)
{
  offset = offset_;
}

void
ControlPoint::on_move_end(const Vector2f& offset_)
{
  offset = offset_;
}

Rectf
ControlPoint::get_bounding_box() const
{
  return Rectf(pos - Vector2f(8.0f, 8.0f), Sizef(16.0f ,16.0f));
}

void
ControlPoint::draw(SceneContext& sc)
{
  sc.control().fill_rect(get_bounding_box().grow(4.0f), Color(0.0f, 0.0f, 0.0f));
  sc.control().fill_rect(get_bounding_box(), Color(1.0f, 0.0f, 0.0f));
}

/* EOF */
