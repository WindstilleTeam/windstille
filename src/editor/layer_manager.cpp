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
#include "layer_manager.hpp"

LayerManager::LayerManager(EditorWindow& editor_)
  : editor(editor_),
    label("Layer Manager", Gtk::ALIGN_LEFT)
{
  treeview.set_headers_clickable();
  treeview.set_headers_visible(true);
  treeview.set_enable_tree_lines();
  treeview.set_reorderable();

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
                                 "    <separator/>"
                                 "  </toolbar>"
                                 "</ui>");
  
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/ToolBar"));

  //treeview.signal_cursor_changed().connect(sigc::mem_fun(*this, &LayerManager::on_cursor_changed));
  //treeview.signal_columns_changed().connect(sigc::mem_fun(*this, &LayerManager::on_columns_changed));

  toolbar.set_icon_size(Gtk::ICON_SIZE_MENU);

  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scrolled.add(treeview);

  pack_start(label,   Gtk::PACK_SHRINK);
  pack_start(toolbar, Gtk::PACK_SHRINK);
  add(scrolled);
  //show_all();
}

LayerManager::~LayerManager()
{
}

void
LayerManager::set_model(SectorModel* model)
{
  if (model)
    {
      treeview.set_model(model->get_layer_tree());

      // Recreate all the columns, since if we don't do that, we lose
      // editability for some reason
      treeview.remove_all_columns();
      treeview.append_column("Type", LayerManagerColumns::instance().type_icon);
      treeview.append_column_editable("Name", LayerManagerColumns::instance().name);
      treeview.append_column_editable("Visible", LayerManagerColumns::instance().visible);
      treeview.append_column_editable("Locked", LayerManagerColumns::instance().locked);

      treeview.expand_all();
      treeview.set_cursor(Gtk::TreeModel::Path("0"));
    }
  else
    {
      treeview.set_model(Glib::RefPtr<Gtk::TreeStore>());
    }
}

void
LayerManager::on_cursor_changed()
{
  Gtk::TreeModel::Path path;
  Gtk::TreeViewColumn* focus_column;
  treeview.get_cursor(path, focus_column);
  
  std::cout << "on_cursor_changed: " << path.to_string() << std::endl;
}

void
LayerManager::on_columns_changed()
{
  std::cout << "LayerManager::on_columns_changed()" << std::endl;
  treeview.expand_all();
}

/* EOF */
