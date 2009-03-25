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

#include <memory>
#include <gtkmm/treestore.h>
#include <vector>

#include "selection.hpp"
#include "object_model.hpp"
#include "math/vector2f.hpp"

class SceneContext;
class ObjectTreeColumns;

class ObjectTreeColumns : public Gtk::TreeModel::ColumnRecord
{
private:
  static ObjectTreeColumns* instance_;

public:
  static ObjectTreeColumns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new ObjectTreeColumns());
  }

  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > type_icon;
  Gtk::TreeModelColumn<Glib::ustring>              name;
  Gtk::TreeModelColumn<bool>                       visible;

private:
  ObjectTreeColumns() {
    add(type_icon); 
    add(name); 
    add(visible); 
  }
};

class SectorModel
{
private:
  typedef std::list<ObjectModelHandle> Objects;

  Gtk::TreeStore::iterator root_it;
  Glib::RefPtr<Gtk::TreeStore> objects_tree;
  Objects objects;

public:
  SectorModel();

  void add(const ObjectModelHandle& object);
  void remove(const ObjectModelHandle& object);
  void draw(SceneContext& sc);
  void update(float delta);

  // void select_objects(const Rectf& rect, bool replace_old_selection = true) const;

  Glib::RefPtr<Gtk::TreeStore> get_objects_tree() { return objects_tree; }

  void on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_deleted(const Gtk::TreeModel::Path& path);
  void on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order);

  ObjectModelHandle get_object_at(const Vector2f& pos) const;
  SelectionHandle   get_selection(const Rectf& rect) const;
  
  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(const ObjectModelHandle& object) const;

private:
  SectorModel(const SectorModel&);
  SectorModel& operator=(const SectorModel&);
};

#endif

/* EOF */
