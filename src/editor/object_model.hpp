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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_MODEL_HPP

#include <glm/glm.hpp>

#include "editor/select_mask.hpp"
#include "util/file_writer.hpp"
#include "editor/control_point.hpp"
#include "editor/snap_data.hpp"
#include "editor/timeline_properties.hpp"
#include "util/file_reader.hpp"

namespace windstille {


class ObjectModel;
class SectorModel;

typedef std::shared_ptr<ObjectModel> ObjectModelHandle;
typedef std::weak_ptr<ObjectModel>   ObjectModelPtr;


class ObjectModel
{
protected:
  std::string name;
  glm::vec2    rel_pos;
  SelectMask  select_mask;

  ObjectModelPtr parent_ptr;

public:
  ObjectModel(ReaderMapping const& reader);
  ObjectModel(std::string const& name, glm::vec2 const& pos);
  virtual ~ObjectModel();

  void set_parent(ObjectModelHandle const& parent_, bool recalc_pos = true);
  ObjectModelHandle get_parent() const;

  std::string get_name() const { return name; }
  std::string get_id() const;

  virtual glm::vec2 get_world_pos() const;
  virtual void set_world_pos(glm::vec2 const& p);

  virtual glm::vec2 get_rel_pos() const { return rel_pos; }
  virtual void     set_rel_pos(glm::vec2 const& rel_pos_);

  virtual SelectMask get_select_mask() const { return select_mask; }
  virtual void   set_select_mask(SelectMask const& select_mask_) { select_mask = select_mask_; }

  virtual SnapData snap_to_object(geom::frect const& rect) const;
  virtual SnapData snap_to_grid(float grid_size) const;

  virtual bool get_hflip() const { return false; }
  virtual bool get_vflip() const { return false; }

  virtual void set_hflip(bool) { }
  virtual void set_vflip(bool) { }

  virtual void sync() {}

  /** Reset scale and rotation to default values */
  virtual void reset();

  virtual bool is_at(glm::vec2 const& pos) const;

  virtual bool is_snappable() const { return true; }

  virtual void draw_select(wstdisplay::SceneContext& sc, bool highlight);

  virtual void draw(wstdisplay::SceneContext& sc);
  virtual void update(float /*delta*/) {}
  virtual geom::frect get_bounding_box() const =0;
  virtual ObjectModelHandle clone() const =0;

  virtual void write(FileWriter& writer) const =0;
  virtual FileWriter& write_member(FileWriter& writer) const;

  virtual void add_control_points(std::vector<ControlPointHandle>& control_points) {}

  virtual void get_property(TimelineProperty property, float& value_out) const;
  virtual void get_property(TimelineProperty property, glm::vec2& value_out) const;

  virtual void set_property(TimelineProperty property, float value);
  virtual void set_property(TimelineProperty property, glm::vec2 const& value);

  /** This lets the object add things to the SceneGraph or do other
      things needed to make it properly visible in the SectorModel */
  virtual void add_to_scenegraph(wstdisplay::DrawableGroup& sg) =0;
};


} // namespace windstille

#endif

/* EOF */
