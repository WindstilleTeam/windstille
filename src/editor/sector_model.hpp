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

#ifndef HEADER_SECTOR_MODEL_HPP
#define HEADER_SECTOR_MODEL_HPP

#include <set>
#include <memory>
#include <gtkmm/treestore.h>
#include <gdkmm/pixbuf.h>
#include <vector>

#include "hard_layer.hpp"
#include "selection.hpp"
#include "object_model.hpp"
#include "math/vector2f.hpp"

class SceneContext;
class LayerManagerColumns;

class LayerManagerColumns : public Gtk::TreeModel::ColumnRecord
{
private:
  static LayerManagerColumns* instance_;

public:
  static LayerManagerColumns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new LayerManagerColumns());
  }

  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > type_icon;
  Gtk::TreeModelColumn<Glib::ustring>              name;
  Gtk::TreeModelColumn<bool>                       visible;
  Gtk::TreeModelColumn<bool>                       locked;
  Gtk::TreeModelColumn<HardLayerHandle>            layer;

private:
  LayerManagerColumns() {
    add(type_icon); 
    add(name); 
    add(visible); 
    add(locked);
    add(layer);
  }
};

class SectorModel
{
private:
  Glib::RefPtr<Gtk::TreeStore> layer_tree;

public:
  //typedef Objects::iterator iterator;
  typedef std::vector<HardLayerHandle> HardLayers;

  SectorModel();

  void draw(SceneContext& sc, const Layers& layers);

  void update(float delta);
  void update(float delta, const Gtk::TreeRow& row);

  void set_all_visible(bool v);
  void set_all_locked(bool v);

  void add_layer(const std::string& name, const Gtk::TreeModel::Path& path = Gtk::TreeModel::Path());
  void delete_layer(const Gtk::TreeModel::Path& path);

  void add(const ObjectModelHandle& object, const Gtk::TreeModel::Path& path);
  void remove(const ObjectModelHandle& object);

  HardLayerHandle get_layer(const Gtk::TreeModel::Path& path) const;
  HardLayers get_layers() const;
  HardLayerHandle get_layer(ObjectModelHandle object);
  
  // void select_objects(const Rectf& rect, bool replace_old_selection = true) const;

  Glib::RefPtr<Gtk::TreeStore> get_layer_tree() { return layer_tree; }

  void on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_deleted(const Gtk::TreeModel::Path& path);
  void on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order);

  ObjectModelHandle get_object_at(const Vector2f& pos, const Layers& layers) const;
  SelectionHandle   get_selection(const Rectf& rect, const Layers& layers) const;
  
  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(const Rectf& object, const std::set<ObjectModelHandle>& ignore_objects) const;

  void load(const std::string& filename);
  void load_layer(const FileReader& filename, const Gtk::TreeModel::Row* parent, 
                  std::map<std::string, ObjectModelHandle>& id_table,
                  std::map<ObjectModelHandle, std::string>& parent_table);
  void write(FileWriter& writer) const;
  void write(FileWriter& writer, const Gtk::TreeRow& row) const;

  void queue_draw();

  //iterator begin() { return objects.begin(); }
  //iterator end() { return objects.end(); }

private:
  SectorModel(const SectorModel&);
  SectorModel& operator=(const SectorModel&);
};

#endif

/* EOF */
