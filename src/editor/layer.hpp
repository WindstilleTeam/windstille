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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_HPP

#include <boost/shared_ptr.hpp>
#include <list>
#include <set>
#include <string>
#include <gtkmm/treemodel.h>

#include "editor/selection.hpp"
#include "editor/object_model.hpp"

class Layer;
typedef boost::shared_ptr<Layer> LayerHandle;

class Layer
{
private:
  typedef std::list<ObjectModelHandle> Objects;

  Objects objects;
  std::string name;
  bool visible;
  bool locked;
    
public:
  typedef Objects::iterator iterator;

  Layer();
  ~Layer();
  
  void set_row_ptr(Gtk::TreeModel::Row* row_ptr);

  iterator begin() { return objects.begin(); }
  iterator end()   { return objects.end(); }

  void set_name(const std::string& str) { name = str; }
  std::string get_name() const { return name; }

  bool is_visible() const { return visible; }
  bool is_locked() const { return locked; }

  // These must not be used, since they don't update the Gtk::TreeModel::Row
  //void set_visible(bool v) { visible = v; }
  //void set_locked(bool v) { locked = v; }

  bool has_object(const ObjectModelHandle& object) const;

  void add(const ObjectModelHandle& object);
  void remove(const ObjectModelHandle& object);

  void draw(SceneContext& sc, const SelectMask& layers);
  void update(float delta);
  void sync(const Gtk::TreeModel::Row& row);

  ObjectModelHandle get_object_at(const Vector2f& pos, const SelectMask& layers) const;
  SelectionHandle   get_selection(const Rectf& rect, const SelectMask& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(const Rectf& rect, const std::set<ObjectModelHandle>& ignore_objects) const;

  void write(FileWriter& writer) const;

  LayerHandle clone() const;
  
private:
  Layer(const Layer&);
  Layer& operator=(const Layer&);
};

#endif

/* EOF */
