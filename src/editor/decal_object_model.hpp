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

#ifndef HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP

#include <geom/quad.hpp>

#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/software_surface.hpp>

#include "editor/control_point.hpp"
#include "editor/object_model.hpp"

namespace windstille {

class DecalObjectModel : public ObjectModel
{
public:
  enum MapType { COLORMAP, LIGHTMAP, HIGHLIGHTMAP };

  static ObjectModelHandle create(std::string const& name, glm::vec2 const& pos, std::string const& path, MapType type);

private:
  std::string path;
  wstdisplay::SurfacePtr surface;
  surf::SoftwareSurface software_surface;
  MapType type;

  /** scale of the object, independed in x and y axis */
  glm::vec2 scale;

  float    angle;

  bool hflip;
  bool vflip;

  std::shared_ptr<wstdisplay::SurfaceDrawable> m_drawable;

public:
  DecalObjectModel(ReaderMapping const& reader);
  DecalObjectModel(std::string const& name, glm::vec2 const& rel_pos,
                   std::string const& path_, MapType type_);
  DecalObjectModel(DecalObjectModel const& rhs);
  ~DecalObjectModel() override;

  void set_scale(glm::vec2 const& scale);
  glm::vec2 get_scale() const { return scale; }

  void set_angle(float angle);
  float get_angle() const { return angle; }

  bool is_snappable() const override { return type == COLORMAP; }

  void draw_select(wstdisplay::SceneContext& sc, bool highlight) override;
  void draw(wstdisplay::SceneContext& sc) override;

  geom::frect get_bounding_box() const override;
  ObjectModelHandle clone() const override;
  void write(FileWriter& writer) const override;

  bool get_hflip() const override { return hflip; }
  bool get_vflip() const override { return vflip; }

  void set_hflip(bool t) override { hflip = t; sync(); }
  void set_vflip(bool t) override { vflip = t; sync(); }

  bool is_at(glm::vec2 const& pos) const override;

  void reset() override;

  void add_control_points(std::vector<ControlPointHandle>& control_points) override;

  void add_to_scenegraph(wstdisplay::DrawableGroup& sg) override;
  void sync() override;

  void set_world_pos(glm::vec2 const& p) override;
  void set_rel_pos(glm::vec2 const& rel_pos_) override;
  void set_select_mask(SelectMask const& select_mask_) override;

  void get_property(TimelineProperty property, float& value_out) const override;
  void get_property(TimelineProperty property, glm::vec2& value_out) const override;

  void set_property(TimelineProperty property, float value) override;
  void set_property(TimelineProperty property, glm::vec2 const& value) override;
};

} // namespace windstille

#endif

/* EOF */
