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

#ifndef HEADER_WINDSTILLE_EDITOR_SECTOR_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_SECTOR_MODEL_HPP

#include <cstddef>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <surf/color.hpp>

#include "editor/layer.hpp"
#include "editor/object_model.hpp"
#include "editor/observable_signal.hpp"
#include "editor/selection.hpp"
#include "editor/timeline_handles.hpp"
#include "util/file_writer.hpp"

namespace windstille {

class SceneContext;
class NavigationGraphModel;
class NavGraphNodeObjectModel;
class NavGraphEdgeObjectModel;

class SectorModel
{
public:
  typedef std::vector<LayerHandle> Layers;
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);

  SectorModel();
  explicit SectorModel(std::string const& filename);
  ~SectorModel();

  void draw(SceneContext& sc, SelectMask const& layers);
  void draw_content(SceneContext& sc);
  void update(float delta);

  void set_all_visible(bool v);
  void set_all_locked(bool v);

  LayerHandle add_layer(std::string const& name, std::size_t index = npos);
  LayerHandle add_layer(LayerHandle layer, std::size_t index = npos);
  void delete_layer(std::size_t index);
  void delete_layer(LayerHandle layer);
  void reverse_layers();
  void move_layer(std::size_t from_index, std::size_t to_index);

  void add(ObjectModelHandle const& object, LayerHandle layer);
  void remove(ObjectModelHandle const& object);
  LayerHandle get_layer(ObjectModelHandle const& object) const;

  void set_ambient_color(surf::Color const& color) { ambient_color = color; }
  surf::Color get_ambient_color() const { return ambient_color; }

  Layers const& get_layers() const { return m_layers; }
  LayerHandle get_layer(std::size_t index) const;
  std::size_t get_layer_index(LayerHandle layer) const;

  ObjectModelHandle get_object_at(glm::vec2 const& pos, SelectMask const& layers) const;
  SelectionHandle get_selection(geom::frect const& rect, SelectMask const& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(ObjectModelHandle object, std::set<ObjectModelHandle> const& ignore_objects) const;

  void write(FileWriter& writer) const;

  NavigationGraphModel& get_nav_graph() const { return *nav_graph; }
  TimelineHandle get_timeline() const { return m_timeline; }

  void delete_navgraph_edges(NavGraphNodeObjectModel& node);

  ObservableSignal<void()>& signal_layers_changed() { return m_signal_layers_changed; }

private:
  std::unique_ptr<NavigationGraphModel> nav_graph;
  Layers m_layers;
  TimelineHandle m_timeline;
  surf::Color ambient_color;
  ObservableSignal<void()> m_signal_layers_changed;

  void notify_layers_changed();

  SectorModel(SectorModel const&) = delete;
  SectorModel& operator=(SectorModel const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
