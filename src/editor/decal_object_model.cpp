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

#include "util/file_reader.hpp"
#include "display/surface.hpp"
#include "display/drawing_parameters.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/scene_context.hpp"
#include "editor/decal_scale_control_point.hpp"
#include "editor/decal_rotate_control_point.hpp"

#include "editor/decal_object_model.hpp"

ObjectModelHandle
DecalObjectModel::create(const std::string& name, const Vector2f& pos,
                         const std::string& path, MapType type)
{
  return ObjectModelHandle(new DecalObjectModel(name, pos, path, type));
}

DecalObjectModel::DecalObjectModel(const FileReader& reader)
  : ObjectModel(reader),
    path(),
    surface(),
    software_surface(),
    type(),
    scale(1.0f, 1.0f),
    angle(0.0f),
    hflip(false),
    vflip(false)
{
  int map_type = 0;
  reader.get("path", path);
  reader.get("type", map_type);
  type = (MapType)map_type;

  reader.get("scale", scale);
  reader.get("angle", angle);
  reader.get("hflip", hflip);
  reader.get("vflip", vflip);
  surface = Surface(Pathname(path));
  software_surface = SoftwareSurface(Pathname(path));
}

DecalObjectModel::DecalObjectModel(const std::string& /*name*/, const Vector2f& rel_pos, 
                                   const std::string& path_, MapType type_)
  : ObjectModel("DecalObjectModel", rel_pos),
    path(path_),
    surface(Pathname(path_)),
    software_surface(Pathname(path_)),
    type(type_),
    scale(1.0f, 1.0f),
    angle(0.0f),
    hflip(false),
    vflip(false)
{
}

DecalObjectModel::~DecalObjectModel()
{
}

void
DecalObjectModel::set_angle(float angle_)
{
  angle = angle_;
}

void
DecalObjectModel::set_scale(const Vector2f& scale_)
{
  scale = scale_;

  // Disallow negative scale
  scale.x = fabsf(scale.x);
  scale.y = fabsf(scale.y);
}

void
DecalObjectModel::draw_select(SceneContext& sc, bool highlight)
{
  Quad quad(get_bounding_box());
  
  quad.rotate(angle);

  if (highlight)
    sc.control().draw_quad(quad, Color(0.5f, 1.0f, 1.0f, 1.0f));
  else
    sc.control().draw_quad(quad, Color(0.5f, 0.5f, 1.0f, 1.0f));
}

void
DecalObjectModel::draw(SceneContext& sc)
{
  ObjectModel::draw(sc);

  Vector2f wo_pos = get_world_pos();
  Vector2f center_offset(-surface.get_width()/2,
                         -surface.get_height()/2);

  DrawingContext* dc = 0; 
  SurfaceDrawingParameters params;
  switch(type)
    {
      case COLORMAP:     dc = &sc.color(); break;
      case LIGHTMAP:     dc = &sc.light();     params.set_blend_func(GL_SRC_ALPHA, GL_ONE); break;
      case HIGHLIGHTMAP: dc = &sc.highlight(); params.set_blend_func(GL_SRC_ALPHA, GL_ONE); break;
    }

  center_offset.x *= scale.x;
  center_offset.y *= scale.y;

  dc->draw(surface, params
           .set_pos(wo_pos + center_offset)
           .set_angle(angle)
           .set_hflip(hflip)
           .set_vflip(vflip)
           .set_scale(scale));
}

Rectf
DecalObjectModel::get_bounding_box() const
{
  Vector2f center_offset(surface.get_width()/2,
                         surface.get_height()/2);

  center_offset.x *= scale.x;
  center_offset.y *= scale.y;

  Rectf r(get_world_pos() - center_offset,
          get_world_pos() + center_offset);
  r.normalize();
  return r;
}

void
DecalObjectModel::reset()
{
  scale = Vector2f(1.0f, 1.0f);
  angle = 0.0f;
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
  writer.write("scale",   scale);
  writer.write("angle",   angle);
  writer.write("hflip",   hflip);
  writer.write("vflip",   vflip);
  writer.end_section();
}

bool
DecalObjectModel::is_at(const Vector2f& pos) const
{
  Vector2f p = pos - get_world_pos();

  // Transform mouse coordinates into coordinates relative to the
  // center of the unscaled and unrotated image
  p = p.rotate(-angle);
  p.x /= scale.x;
  p.y /= scale.y;

  if (fabsf(p.x) < surface.get_width()/2 &&
      fabsf(p.y) < surface.get_height()/2)
    {
      if (hflip)
        p.x = -p.x;

      if (vflip)
        p.y = -p.y;

      return software_surface.is_at(static_cast<int>(p.x + surface.get_width()/2),
                                    static_cast<int>(p.y + surface.get_height()/2));
    }
  else
    {
      return false;
    }
}

void
DecalObjectModel::add_control_points(std::vector<ControlPointHandle>& control_points)
{
  float w = surface.get_width()/2  * scale.x;
  float h = surface.get_height()/2 * scale.y;

  Rectf rect(-w, -h, w, h);
  Quad quad1(rect);
  quad1.rotate(angle);

  Quad quad2(rect.grow(32));
  quad2.rotate(angle);

  Quad quad3(Vector2f( 0, -h),
             Vector2f( w,  0),
             Vector2f( 0,  h),
             Vector2f(-w,  0));
  quad3.rotate(angle);

  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 0*math::pi/2, get_world_pos() + quad1.p1)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 1*math::pi/2, get_world_pos() + quad1.p2)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 2*math::pi/2, get_world_pos() + quad1.p3)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 3*math::pi/2, get_world_pos() + quad1.p4)));

  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, math::pi/4 + angle + 0*math::pi/2, get_world_pos() + quad3.p1, false, true)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, math::pi/4 + angle + 1*math::pi/2, get_world_pos() + quad3.p2, true,  false)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, math::pi/4 + angle + 2*math::pi/2, get_world_pos() + quad3.p3, false, true)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, math::pi/4 + angle + 3*math::pi/2, get_world_pos() + quad3.p4, true,  false)));

  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 0*math::pi/2, get_world_pos() + quad2.p1)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 1*math::pi/2, get_world_pos() + quad2.p2)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 2*math::pi/2, get_world_pos() + quad2.p3)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 3*math::pi/2, get_world_pos() + quad2.p4)));
}

/* EOF */
