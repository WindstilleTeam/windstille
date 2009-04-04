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
#include "decal_object_model.hpp"

ObjectModelHandle
DecalObjectModel::create(const std::string& name, const Vector2f& pos,
                         const std::string& path, MapType type)
{
  return ObjectModelHandle(new DecalObjectModel(name, pos, path, type));
}

DecalObjectModel::DecalObjectModel(const FileReader& reader)
  : ObjectModel(reader),
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
  surface = Surface(path);
}

DecalObjectModel::DecalObjectModel(const std::string& name, const Vector2f& rel_pos, 
                                   const std::string& path_, MapType type_)
  : ObjectModel("DecalObjectModel", rel_pos),
    path(path_),
    surface(path_),
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

class DecalRotateControlPoint : public ControlPoint
{
private:
  DecalObjectModel* object;
  float ctrl_angle;
  float orig_angle;
  Vector2f center;

public:
  DecalRotateControlPoint(DecalObjectModel* object_, float ctrl_angle_, const Vector2f& pos_)
    : ControlPoint(Surface("editor/rotate_handle.png"), pos_),
      object(object_),
      ctrl_angle(ctrl_angle_),
      orig_angle(object->get_angle()),
      center(object->get_world_pos())
  {
  }

  void on_move_start() 
  {
  }

  void on_move_update(const Vector2f& offset_) 
  {
    offset = offset_;

    Vector2f base = pos - center;
    float base_angle = atan2(base.y, base.x);

    Vector2f current = (pos+offset) - center;
    float current_angle = atan2(current.y, current.x);

    float new_angle = orig_angle + current_angle - base_angle;
    object->set_angle(new_angle);
  }
  
  void on_move_end(const Vector2f& offset_)
  {
    on_move_update(offset_);
  }

  void draw(SceneContext& sc)
  {
    Rectf rect = get_bounding_box();
    rect += offset;
    sc.control().draw_control(surface, pos, ctrl_angle);
  }
};

class DecalScaleControlPoint : public ControlPoint
{
private:
  DecalObjectModel* object;
  float ctrl_angle;
  Vector2f orig_scale;

public:
  DecalScaleControlPoint(DecalObjectModel* object_, float ctrl_angle_, const Vector2f& pos_)
    : ControlPoint(Surface("editor/scale_handle.png"), pos_),
      object(object_),
      ctrl_angle(ctrl_angle_),
      orig_scale(object_->get_scale())
  {}

  void on_move_start() 
  {
  }

  void on_move_update(const Vector2f& offset_) 
  {
    offset = offset_; 

    Vector2f start   = pos - object->get_world_pos();
    Vector2f current = (pos + offset) - object->get_world_pos();

    start    = start.rotate(-object->get_angle());
    current = current.rotate(-object->get_angle());
    
    //std::cout << "on_move_update: " << current.x << " " << current.y << std::endl;  

    Vector2f new_scale = current / start;

    new_scale.x *= orig_scale.x;
    new_scale.y *= orig_scale.y;

    object->set_scale(new_scale);
  }
  
  void on_move_end(const Vector2f& offset_)
  {
    on_move_update(offset_);
  }

  void draw(SceneContext& sc)
  {
    Rectf rect = get_bounding_box();
    rect += offset;
    sc.control().draw_control(surface, pos, ctrl_angle);
  }
};

bool
DecalObjectModel::is_at(const Vector2f& pos) const
{
  Vector2f p = pos - get_world_pos();
  p = p.rotate(-angle);
  p.x /= scale.x;
  p.y /= scale.y;

  if (fabsf(p.x) < surface.get_width()/2 &&
      fabsf(p.y) < surface.get_height()/2)
    {
      return true;
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

  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 0*M_PI/2, get_world_pos() + quad1.p1)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 1*M_PI/2, get_world_pos() + quad1.p2)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 2*M_PI/2, get_world_pos() + quad1.p3)));
  control_points.push_back(ControlPointHandle(new DecalScaleControlPoint(this, angle + 3*M_PI/2, get_world_pos() + quad1.p4)));

  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 0*M_PI/2, get_world_pos() + quad2.p1)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 1*M_PI/2, get_world_pos() + quad2.p2)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 2*M_PI/2, get_world_pos() + quad2.p3)));
  control_points.push_back(ControlPointHandle(new DecalRotateControlPoint(this, angle + 3*M_PI/2, get_world_pos() + quad2.p4)));
}

/* EOF */
