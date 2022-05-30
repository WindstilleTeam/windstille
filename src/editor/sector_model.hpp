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

#include <surf/color.hpp>
#include "editor/layer.hpp"
#include "editor/object_model.hpp"
#include "editor/selection.hpp"
#include "editor/timeline_handles.hpp"
#include <glm/glm.hpp>

namespace windstille {


class NavigationGraphModel;
class NavGraphNodeObjectModel;
class NavGraphEdgeObjectModel;


class SectorModel
{
private:
  std::unique_ptr<NavigationGraphModel> nav_graph;
  Glib::RefPtr<Gtk::ListStore> layer_tree;
  TimelineHandle m_timeline;
  surf::Color ambient_color;

public:
  typedef std::vector<LayerHandle> Layers;

  SectorModel();
  SectorModel(std::string const& filename);
  ~SectorModel();

  void draw(wstdisplay::SceneContext& sc, SelectMask const& layers);

  void update(float delta);

  void set_all_visible(bool v);
  void set_all_locked(bool v);

  void add_layer(std::string const& name, Gtk::TreeModel::Path const& path = Gtk::TreeModel::Path());
  void add_layer(LayerHandle layer, Gtk::TreeModel::Path const& path = Gtk::TreeModel::Path());
  void delete_layer(Gtk::TreeModel::Path const& path);
  void reverse_layers();

  void add(ObjectModelHandle const& object, Gtk::TreeModel::Path const& path);
  void remove(ObjectModelHandle const& object);
  LayerHandle get_layer(ObjectModelHandle const& object) const;

  void  set_ambient_color(surf::Color const& color) { ambient_color = color; }
  surf::Color get_ambient_color() const { return ambient_color; }

  LayerHandle get_layer(Gtk::TreeModel::Path const& path) const;
  Layers get_layers() const;
  LayerHandle get_layer(ObjectModelHandle object);

  Glib::RefPtr<Gtk::ListStore> get_layer_tree() { return layer_tree; }

  void on_row_changed(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_row_deleted(Gtk::TreeModel::Path const& path);
  void on_row_has_child_toggled(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_row_inserted(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_rows_reordered(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter, int* new_order);

  ObjectModelHandle get_object_at(glm::vec2 const& pos, SelectMask const& layers) const;
  SelectionHandle   get_selection(geom::frect const& rect, SelectMask const& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(ObjectModelHandle object, std::set<ObjectModelHandle> const& ignore_objects) const;

  void write(FileWriter& writer) const;

  NavigationGraphModel& get_nav_graph() const { return *nav_graph; }
  TimelineHandle get_timeline() const { return m_timeline; }

  void delete_navgraph_edges(NavGraphNodeObjectModel& node);

  void rebuild_scene_graph(wstdisplay::DrawableGroup& sg);

private:
  void register_callbacks();

private:
  SectorModel(SectorModel const&);
  SectorModel& operator=(SectorModel const&);
};


} // namespace windstille

#endif

/* EOF */
