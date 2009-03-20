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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_TREE_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_TREE_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/actiongroup.h>

class ObjectTree : public Gtk::VBox
{
private:
  Gtk::Label label;
  Gtk::ScrolledWindow scrolled;
  Gtk::TreeView treeview;

  Glib::RefPtr<Gtk::UIManager>   ui_manager;
  Glib::RefPtr<Gtk::ActionGroup> action_group;

public:
  ObjectTree();
  virtual ~ObjectTree();
  
  void on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_deleted(const Gtk::TreeModel::Path& path);
  void on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order);

private:
  ObjectTree(const ObjectTree&);
  ObjectTree& operator=(const ObjectTree&);
};

#endif

/* EOF */
