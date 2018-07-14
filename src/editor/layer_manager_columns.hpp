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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_MANAGER_COLUMNS_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_MANAGER_COLUMNS_HPP

#include <gdkmm/pixbuf.h>
#include <gtkmm/treemodelcolumn.h>

#include "layer.hpp"

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
  Gtk::TreeModelColumn<LayerHandle>                layer;

private:
  LayerManagerColumns()
    : type_icon(),
      name(),
      visible(),
      locked(),
      layer()
  {
    add(type_icon);
    add(name);
    add(visible);
    add(locked);
    add(layer);
  }
};

#endif

/* EOF */
