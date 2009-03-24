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

ObjectModelHandle
ObjectModel::create(const std::string& name, const std::string& path, const Vector2f& pos,
                    MapType type)
{
  return ObjectModelHandle(new ObjectModel(name, path, pos, type));
}

ObjectModel::ObjectModel(const std::string& name_, const std::string& path_, const Vector2f& rel_pos_, MapType type_)
  : name(name_),
    path(path_),
    rel_pos(rel_pos_),
    type(type_)
{
  std::cout << "Path: " << path << std::endl;
  surface = Surface(path.substr(5)); // cut "data/" part
}

ObjectModel::~ObjectModel()
{
}

ObjectModelHandle
ObjectModel::clone() const
{
  return ObjectModel::create(name, path, get_world_pos(), type);
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

  Vector2f center_offset(-surface.get_width()/2,
                         -surface.get_height()/2);

  switch(type)
    {
      case COLORMAP:
        sc.color().draw(surface, wo_pos + center_offset); 
        break;

      case LIGHTMAP:
        sc.light().draw(surface, SurfaceDrawingParameters()
                        .set_pos(wo_pos + center_offset)
                        .set_blend_func(GL_SRC_ALPHA, GL_ONE)); 
        break;

      case HIGHLIGHTMAP:
        sc.highlight().draw(surface, SurfaceDrawingParameters()
                            .set_pos(wo_pos + center_offset)
                            .set_blend_func(GL_SRC_ALPHA, GL_ONE)); 
        break;
    }
  sc.control().fill_rect(Rectf(wo_pos - Vector2f(8, 8), Sizef(16, 16)), Color(1,0,0));
}

Rectf
ObjectModel::get_bounding_box() const
{
  Vector2f center_offset(-surface.get_width()/2,
                         -surface.get_height()/2);

  return Rectf(get_world_pos() + center_offset, Sizef(surface.get_width(), surface.get_height()));
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

/* EOF */
