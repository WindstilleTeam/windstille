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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_HPP

#include <gtkmm/treemodel.h>

#include "editor/selection.hpp"

namespace windstille {


class Layer;
typedef std::shared_ptr<Layer> LayerHandle;


class Layer
{
private:
  typedef std::list<ObjectModelHandle> Objects;

  SectorModel& m_sector;
  Objects objects;
  std::string name;
  bool visible;
  bool locked;

public:
  typedef Objects::iterator iterator;
  typedef Objects::const_iterator const_iterator;

  Layer(SectorModel& sector);
  ~Layer();

  void set_row_ptr(Gtk::TreeModel::Row* row_ptr);

  iterator begin() { return objects.begin(); }
  iterator end()   { return objects.end(); }

  const_iterator begin() const { return objects.begin(); }
  const_iterator end()   const { return objects.end(); }

  void set_name(std::string const& str) { name = str; }
  std::string get_name() const { return name; }

  bool is_visible() const { return visible; }
  bool is_locked() const { return locked; }

  // These must not be used, since they don't update the Gtk::TreeModel::Row
  //void set_visible(bool v) { visible = v; }
  //void set_locked(bool v) { locked = v; }

  bool has_object(ObjectModelHandle const& object) const;

  void add(ObjectModelHandle const& object);
  void remove(ObjectModelHandle const& object);
  iterator erase(iterator it);

  void draw(wstdisplay::SceneContext& sc, SelectMask const& layers);
  void update(float delta);
  void sync(Gtk::TreeModel::Row const& row);

  ObjectModelHandle get_object_at(glm::vec2 const& pos, SelectMask const& layers) const;
  SelectionHandle   get_selection(geom::frect const& rect, SelectMask const& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(ObjectModelHandle object, std::set<ObjectModelHandle> const& ignore_objects) const;

  void write(FileWriter& writer) const;

private:
  Layer(Layer const&);
  Layer& operator=(Layer const&);
};


} // namespace windstille

#endif

/* EOF */
