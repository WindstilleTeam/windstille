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

#include "util/file_reader.hpp"
#include "display/surface.hpp"
#include "display/drawing_parameters.hpp"
#include "display/scene_context.hpp"
#include "decal_object_model.hpp"

ObjectModelHandle
DecalObjectModel::create(const std::string& name, const Vector2f& pos,
                         const std::string& path, MapType type)
{
  return ObjectModelHandle(new DecalObjectModel(name, pos, path, type));
}

DecalObjectModel::DecalObjectModel(const FileReader& reader)
  : ObjectModel(reader)
{
  int map_type = 0;
  reader.get("path", path);
  reader.get("type", map_type);
  type = (MapType)map_type;
  surface = Surface(path);

  quad = Quad(0, 0, surface.get_width(), surface.get_height());
}

DecalObjectModel::DecalObjectModel(const std::string& name, const Vector2f& rel_pos, 
                                   const std::string& path_, MapType type_)
  : ObjectModel("DecalObjectModel", rel_pos),
    path(path_),
    surface(path_),
    type(type_)
{
  quad = Quad(0, 0, surface.get_width(), surface.get_height());
}

DecalObjectModel::~DecalObjectModel()
{
}

void
DecalObjectModel::draw(SceneContext& sc)
{
  ObjectModel::draw(sc);

  Vector2f wo_pos = get_world_pos();
  Vector2f center_offset(-surface.get_width()/2,
                         -surface.get_height()/2);

  switch(type)
    {
      case COLORMAP:
        sc.color().draw(surface, wo_pos + center_offset, quad,
                        DrawingParameters());
        break;

      case LIGHTMAP:
        sc.light().draw(surface, wo_pos + center_offset, quad, 
                        DrawingParameters().set_blend_func(GL_SRC_ALPHA, GL_ONE));
        break;

      case HIGHLIGHTMAP:
        sc.highlight().draw(surface, wo_pos + center_offset, quad, 
                            DrawingParameters().set_blend_func(GL_SRC_ALPHA, GL_ONE));
        break;
    }
}

Rectf
DecalObjectModel::get_bounding_box() const
{
  Vector2f center_offset(-surface.get_width()/2,
                         -surface.get_height()/2);

  return quad.get_bounding_box() + get_world_pos() + center_offset;
}

ObjectModelHandle
DecalObjectModel::clone() const
{
  return ObjectModelHandle(new DecalObjectModel(*this));
}

void
DecalObjectModel::write(FileWriter& writer) const
{
  writer.start_section("decal");
  ObjectModel::write_member(writer);
  writer.write("path",    path);
  writer.write("type",    type);
  writer.end_section();
}

/* EOF */
