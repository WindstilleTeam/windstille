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

#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treemodelcolumn.h>

#include "editor_window.hpp"
#include "sector_model.hpp"
#include "object_tree.hpp"

ObjectTree::ObjectTree(EditorWindow& editor_)
  : editor(editor_),
    label("Scene Tree", Gtk::ALIGN_LEFT)
{
  //treeview.set_headers_clickable();
  treeview.set_headers_visible(false);
  treeview.set_enable_tree_lines();
  treeview.set_reorderable();

  treeview.append_column("Type", ObjectTreeColumns::instance().type_icon);
  treeview.append_column("Name", ObjectTreeColumns::instance().name);
  treeview.append_column_editable("Visible", ObjectTreeColumns::instance().visible);

  treeview.expand_all();

  ui_manager   = Gtk::UIManager::create();
  action_group = Gtk::ActionGroup::create();
  
  action_group->add(Gtk::Action::create("NewLayer", Gtk::Stock::NEW),
                    sigc::mem_fun(editor, &EditorWindow::on_new_layer));
  action_group->add(Gtk::Action::create("DeleteLayer", Gtk::Stock::DELETE),
                    sigc::mem_fun(editor, &EditorWindow::on_delete_layer));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar  name='ToolBar'>"
                                 "    <toolitem action='NewLayer'/>"
                                 "    <toolitem action='DeleteLayer'/>"
                                 "  </toolbar>"
                                 "</ui>");
  
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/ToolBar"));

  toolbar.set_icon_size(Gtk::ICON_SIZE_MENU);

  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scrolled.add(treeview);

  pack_start(label, Gtk::PACK_SHRINK);
  pack_start(toolbar, Gtk::PACK_SHRINK);
  add(scrolled);
  //show_all();
}

ObjectTree::~ObjectTree()
{
}

void
ObjectTree::set_model(SectorModel* model)
{
  if (model)
    {
      treeview.set_model(model->get_objects_tree());
      treeview.expand_all();
    }
  else
    {
      treeview.set_model(Glib::RefPtr<Gtk::TreeStore>());
    }
}

/* EOF */
