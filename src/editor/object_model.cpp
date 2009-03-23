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
#include "object_model.hpp"

ObjectModelHandle
ObjectModel::create(const std::string& name, const std::string& path, const Vector2f& pos)
{
  return ObjectModelHandle(new ObjectModel(name, path, pos));
}

ObjectModel::ObjectModel(const std::string& name_, const std::string& path, const Vector2f& pos_)
  : name(name_),
    pos(pos_)
{
  std::cout << "Path: " << path << std::endl;
  surface = Surface(path.substr(5)); // cut "data/" part

  pos.x -= surface.get_width()/2;
  pos.y -= surface.get_height()/2;
}

ObjectModel::~ObjectModel()
{
}

void
ObjectModel::draw(SceneContext& sc)
{
  sc.color().draw(surface, pos.x+move_offset.x, pos.y+move_offset.y);
  sc.control().fill_rect(Rectf(pos - Vector2f(8, 8) + move_offset, Sizef(16, 16)), Color(1,0,0));
}

Rectf
ObjectModel::get_bounding_box() const
{
  return Rectf(pos+move_offset, Sizef(surface.get_width(), surface.get_height()));
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
  pos += move_offset;
  move_offset = Vector2f(0,0);
}

/* EOF */
