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

#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treemodelcolumn.h>
#include "object_tree.hpp"

class ObjectTreeColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<Glib::ustring> type;
  Gtk::TreeModelColumn<Glib::ustring> name;
  Gtk::TreeModelColumn<bool>          visible;

  ObjectTreeColumns() {
    add(type); 
    add(name); 
    add(visible); 
  }
};

ObjectTree::ObjectTree()
  : label("Scene Tree", Gtk::ALIGN_LEFT)
{
  ObjectTreeColumns columns;
  Glib::RefPtr<Gtk::TreeStore> list_store = Gtk::TreeStore::create(columns);

  Gtk::TreeStore::iterator root = list_store->append();
  (*root)[columns.type]    = Glib::ustring("Hello World");
  (*root)[columns.name]    = Glib::ustring("Hello World");
  (*root)[columns.visible] = false;

  for(int i = 0; i < 10; ++i)
    {
      Gtk::TreeStore::iterator it  = list_store->append(root->children());
      (*it)[columns.type]    = Glib::ustring("Hello World");
      (*it)[columns.name]    = Glib::ustring("Hello World");
      (*it)[columns.visible] = false;

      Gtk::TreeModel::iterator it2  = list_store->append(it->children());
      (*it2)[columns.type]    = Glib::ustring("Hello World");
      (*it2)[columns.name]    = Glib::ustring("Hello World");
      (*it2)[columns.visible] = false;
    }

  //treeview.set_headers_clickable();
  treeview.set_headers_visible(false);
  treeview.set_enable_tree_lines();
  treeview.set_model(list_store);

  treeview.append_column("Type", columns.type);
  treeview.append_column("Name", columns.name);
  treeview.append_column_editable("Visible", columns.visible);

  treeview.expand_all();

  ui_manager   = Gtk::UIManager::create();
  action_group = Gtk::ActionGroup::create();
  
  action_group->add(Gtk::Action::create("ExpandAll", Gtk::Stock::MEDIA_PLAY));
  action_group->add(Gtk::Action::create("ShowAll",   Gtk::Stock::ZOOM_100));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar  name='ToolBar'>"
                                 "    <toolitem action='ExpandAll'/>"
                                 "    <toolitem action='ShowAll'/>"
                                 "  </toolbar>"
                                 "</ui>");
  
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/ToolBar"));

  toolbar.set_icon_size(Gtk::ICON_SIZE_MENU);

  scrolled.add(treeview);

  pack_start(label, Gtk::PACK_SHRINK);
  pack_start(toolbar, Gtk::PACK_SHRINK);
  add(scrolled);

  show_all();
}

ObjectTree::~ObjectTree()
{
}

/* EOF */
