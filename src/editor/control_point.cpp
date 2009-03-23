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

#include "control_point.hpp"

ControlPoint::ControlPoint()
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

/* EOF */
