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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_SELECTOR_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_SELECTOR_HPP

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gdkmm/dragcontext.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/iconview.h>

#include "util/pathname.hpp"

class EditorWindow;

class ObjectSelector : public Gtk::VBox
{
private:
  EditorWindow& editor;

  Gtk::Label label;
  Gtk::ScrolledWindow scrolled;
  Gtk::IconView iconview;
  Glib::RefPtr<Gtk::ListStore>       list_store;
  Glib::RefPtr<Gtk::TreeModelFilter> list_filter;

  Gtk::ComboBoxText filter_box;

  struct ComboBoxEntry {
    std::string name;
    unsigned int filter_mask;

    ComboBoxEntry(const std::string& name_,
                  unsigned int filter_mask_)
      : name(name_),
        filter_mask(filter_mask_)
    {}
  };

  std::vector<ComboBoxEntry> filter_entries;

  class Columns;

  enum {
    OBJECT_GROUP_LIGHT      = (1u<<0),
    OBJECT_GROUP_HIGHLIGHT  = (1u<<1),
    OBJECT_GROUP_DECAL      = (1u<<2),

    OBJECT_GROUP_GROUND     = (1u<<3),
    OBJECT_GROUP_BACKGROUND = (1u<<4),
    OBJECT_GROUP_WALL       = (1u<<5),
    OBJECT_GROUP_OBJECT     = (1u<<6),
    OBJECT_GROUP_DECOR      = (1u<<7),
    OBJECT_GROUP_DOOR       = (1u<<8),
    OBJECT_GROUP_LAMP       = (1u<<9),

    OBJECT_GROUP_CHARACTER  = (1u<<10),
    OBJECT_GROUP_CREATURE   = (1u<<11),

    OBJECT_GROUP_PARTICLESYSTEM = (1u<<12),

    OBJECT_GROUP_ALL        = ~0u
  };

  unsigned int filter_mask;

public:
  ObjectSelector(EditorWindow& editor);
  ~ObjectSelector() override;

  void add_decal(const Glib::RefPtr<Gdk::Pixbuf>& icon,
                 const Pathname& pathname,
                 const std::string& url,
                 unsigned int filter);

  void add_decals_from_directory(const Pathname& pathname, unsigned int filter);

  void populate();
  void refresh();
  bool filter(const Gtk::TreeModel::const_iterator& it);

protected:
  void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context) override;
  void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context,
                        Gtk::SelectionData& selection_data,
                        guint info, guint time) override;

  void on_filter_changed();

private:
  ObjectSelector(const ObjectSelector&);
  ObjectSelector& operator=(const ObjectSelector&);
};

#endif

/* EOF */
