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

#include <set>
#include <memory>
#include <gtkmm/liststore.h>
#include <gdkmm/pixbuf.h>
#include <vector>

#include "display/color.hpp"
#include "editor/layer.hpp"
#include "editor/object_model.hpp"
#include "editor/selection.hpp"
#include "editor/timeline_handles.hpp"
#include <glm/glm.hpp>

class NavigationGraphModel;
class NavGraphNodeObjectModel;
class NavGraphEdgeObjectModel;
class SceneGraph;
class SceneContext;

class SectorModel
{
private:
  std::unique_ptr<NavigationGraphModel> nav_graph;
  Glib::RefPtr<Gtk::ListStore> layer_tree;
  TimelineHandle m_timeline;
  RGBAf ambient_color;

public:
  typedef std::vector<LayerHandle> Layers;

  SectorModel();
  SectorModel(const std::string& filename);
  ~SectorModel();

  void draw(SceneContext& sc, const SelectMask& layers);

  void update(float delta);

  void set_all_visible(bool v);
  void set_all_locked(bool v);

  void add_layer(const std::string& name, const Gtk::TreeModel::Path& path = Gtk::TreeModel::Path());
  void add_layer(LayerHandle layer, const Gtk::TreeModel::Path& path = Gtk::TreeModel::Path());
  void delete_layer(const Gtk::TreeModel::Path& path);
  void reverse_layers();

  void add(const ObjectModelHandle& object, const Gtk::TreeModel::Path& path);
  void remove(const ObjectModelHandle& object);
  LayerHandle get_layer(const ObjectModelHandle& object) const;

  void  set_ambient_color(const RGBAf& color) { ambient_color = color; }
  RGBAf get_ambient_color() const { return ambient_color; }

  LayerHandle get_layer(const Gtk::TreeModel::Path& path) const;
  Layers get_layers() const;
  LayerHandle get_layer(ObjectModelHandle object);

  Glib::RefPtr<Gtk::ListStore> get_layer_tree() { return layer_tree; }

  void on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_deleted(const Gtk::TreeModel::Path& path);
  void on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order);

  ObjectModelHandle get_object_at(const glm::vec2& pos, const SelectMask& layers) const;
  SelectionHandle   get_selection(const geom::frect& rect, const SelectMask& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(ObjectModelHandle object, const std::set<ObjectModelHandle>& ignore_objects) const;

  void write(FileWriter& writer) const;

  NavigationGraphModel& get_nav_graph() const { return *nav_graph; }
  TimelineHandle get_timeline() const { return m_timeline; }

  void delete_navgraph_edges(NavGraphNodeObjectModel& node);

  void rebuild_scene_graph(DrawableGroup& sg);

private:
  void register_callbacks();

private:
  SectorModel(const SectorModel&);
  SectorModel& operator=(const SectorModel&);
};

#endif

/* EOF */
