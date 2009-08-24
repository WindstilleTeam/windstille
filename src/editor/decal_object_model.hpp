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

#ifndef HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP

#include "math/quad.hpp"
#include "editor/control_point.hpp"
#include "display/surface.hpp"
#include "display/software_surface.hpp"
#include "editor/object_model.hpp"

class DecalObjectModel : public ObjectModel
{
public:
  enum MapType { COLORMAP, LIGHTMAP, HIGHLIGHTMAP };

  static ObjectModelHandle create(const std::string& name, const Vector2f& pos, const std::string& path, MapType type);

private:
  std::string path;
  Surface surface;
  SoftwareSurface software_surface;
  MapType type;

  /** scale of the object, independed in x and y axis */
  Vector2f scale;

  float    angle;

  bool hflip;
  bool vflip;

public:
  DecalObjectModel(const FileReader& reader);
  DecalObjectModel(const std::string& name, const Vector2f& rel_pos, 
                   const std::string& path_, MapType type_);
  ~DecalObjectModel();

  void set_scale(const Vector2f& scale);
  Vector2f get_scale() const { return scale; }

  void set_angle(float angle);
  float get_angle() const { return angle; }

  bool is_snappable() const { return type == COLORMAP; }

  void draw_select(SceneContext& sc, bool highlight);
  void draw(SceneContext& sc);

  Rectf get_bounding_box() const;
  ObjectModelHandle clone() const;
  void write(FileWriter& writer) const;

  bool get_hflip() const { return hflip; }
  bool get_vflip() const { return vflip; }

  void set_hflip(bool t) { hflip = t; }
  void set_vflip(bool t) { vflip = t; }

  bool is_at(const Vector2f& pos) const;

  void reset();

  void add_control_points(std::vector<ControlPointHandle>& control_points);
};

#endif

/* EOF */
