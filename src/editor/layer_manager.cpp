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

#include "editor/layer_manager.hpp"

#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodelcolumn.h>

#include "editor/editor_window.hpp"
#include "editor/sector_model.hpp"
#include "editor/layer_manager_columns.hpp"

LayerManager::LayerManager(EditorWindow& editor_)
  : editor(editor_),
    label("Layer Manager", Gtk::ALIGN_LEFT),
    scrolled(),
    treeview(),
    auto_lock()
{
  treeview.set_headers_clickable();
  treeview.set_headers_visible(true);
  treeview.set_enable_tree_lines();
  treeview.set_reorderable();

  Glib::RefPtr<Gtk::UIManager>   ui_manager   = editor.get_ui_manager();
  Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("MenuLayer",   "_Layer"));
  action_group->add(Gtk::Action::create("NewLayer", Gtk::Stock::NEW),
                    sigc::mem_fun(editor, &EditorWindow::on_new_layer));
  action_group->add(Gtk::Action::create("DeleteLayer", Gtk::Stock::DELETE),
                    sigc::mem_fun(editor, &EditorWindow::on_delete_layer));
  action_group->add(Gtk::Action::create("ReverseLayers", Gtk::Stock::SORT_ASCENDING),
                    sigc::mem_fun(editor, &EditorWindow::on_reverse_layers));

  action_group->add(Gtk::Action::create_with_icon_name("ShowAllLayer", "show_all", "Show All", "Show All Layer"),
                    sigc::bind(sigc::mem_fun(editor, &EditorWindow::on_show_all), true));
  action_group->add(Gtk::Action::create_with_icon_name("HideAllLayer", "hide_all", "Hide All", "Hide All Layer"),
                    sigc::bind(sigc::mem_fun(editor, &EditorWindow::on_show_all), false));
  action_group->add(Gtk::Action::create_with_icon_name("LockAllLayer", "lock_all", "Lock All", "Lock All Layer"),
                    sigc::bind(sigc::mem_fun(editor, &EditorWindow::on_lock_all), true));
  action_group->add(Gtk::Action::create_with_icon_name("UnlockAllLayer", "unlock_all", "Unlock All", "Unlock All Layer"),
                    sigc::bind(sigc::mem_fun(editor, &EditorWindow::on_lock_all), false));

  auto_lock = Gtk::ToggleAction::create_with_icon_name("AutoLockLayer", "auto_lock", "Auto Lock All", "All layers except the current ones are treated as locked");
  action_group->add(auto_lock,
                    sigc::bind(sigc::mem_fun(*this, &LayerManager::on_auto_lock), auto_lock));
  //sigc::bind(sigc::mem_fun(editor, &EditorWindow::on_auto_lock), auto_lock));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar  name='LayerManagerToolBar'>"
                                 "    <toolitem action='NewLayer'/>"
                                 "    <toolitem action='DeleteLayer'/>"
                                 "    <separator/>"
                                 "    <toolitem action='ShowAllLayer'/>"
                                 "    <toolitem action='HideAllLayer'/>"
                                 "    <separator/>"
                                 "    <toolitem action='LockAllLayer'/>"
                                 "    <toolitem action='UnlockAllLayer'/>"
                                 "    <separator/>"
                                 "    <toolitem action='AutoLockLayer'/>"
                                 "  </toolbar>"
                                 "</ui>");
  
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/LayerManagerToolBar"));

  treeview.signal_cursor_changed().connect(sigc::mem_fun(*this, &LayerManager::on_cursor_changed));
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
    treeview.set_model(Glib::RefPtr<Gtk::ListStore>());
  }
}

void
LayerManager::on_cursor_changed()
{
  if (auto_lock->get_active())
  {
    Gtk::TreeModel::Path path_;
    Gtk::TreeViewColumn* focus_column;
    treeview.get_cursor(path_, focus_column);
      
    if (!path_.gobj())
    {
      std::cout << "LayerManager::on_cursor_changed(): Error: Couldn't get path" << std::endl;
    }
    else
    {
      //std::cout << "on_cursor_changed: " << path.to_string() << std::endl;
      Gtk::TreeModel::iterator it = treeview.get_model()->get_iter(path_);
      if (it)
      {
	EditorWindow::current()->on_lock_all(true);
	(*it)[LayerManagerColumns::instance().locked] = false;
	((LayerHandle)(*it)[LayerManagerColumns::instance().layer])->sync(*it);
      }
    }
  }
}

void
LayerManager::on_columns_changed()
{
  //std::cout << "LayerManager::on_columns_changed()" << std::endl;
  treeview.expand_all();
}

void
LayerManager::on_auto_lock(Glib::RefPtr<Gtk::ToggleAction> /*auto_lock*/)
{
  on_cursor_changed();
}

/* EOF */
