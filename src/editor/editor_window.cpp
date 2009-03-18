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
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/stock.h>

#include "about_window.hpp"
#include "editor_window.hpp"

EditorWindow::EditorWindow()
{
  set_title("Windstille Editor");

  Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "      <menuitem action='Open'/>"
    "      <menuitem action='Save'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='MenuEdit'>"
    "      <menuitem action='Cut'/>"
    "      <menuitem action='Copy'/>"
    "      <menuitem action='Paste'/>"
    "    </menu>"
    "    <menu action='MenuView'>"
    "      <menuitem action='ZoomIn'/>"
    "      <menuitem action='ZoomOut'/>"
    "      <menuitem action='Zoom100'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='New'/>"
    "    <toolitem action='Open'/>"
    "    <toolitem action='Save'/>"
    "    <separator/>"
    "    <toolitem action='Cut'/>"
    "    <toolitem action='Copy'/>"
    "    <toolitem action='Paste'/>"
    "    <separator/>"
    "    <toolitem action='ZoomIn'/>"
    "    <toolitem action='ZoomOut'/>"
    "    <toolitem action='Zoom100'/>"
    "    <separator/>"
    "    <toolitem action='Play'/>"
    "    <separator/>"
    "    <toolitem action='About'/>"
    "    <toolitem action='Quit'/>"
    "  </toolbar>"
    "  <toolbar  name='ToolBox'>"
    "    <toolitem action='Open'/>"
    "    <toolitem action='Quit'/>"
    "  </toolbar>"
    "</ui>";

  ui_manager   = Gtk::UIManager::create();
  action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("MenuFile",    "_File"));
  action_group->add(Gtk::Action::create("New",         Gtk::Stock::NEW));
  action_group->add(Gtk::Action::create("Open",        Gtk::Stock::OPEN));
  action_group->add(Gtk::Action::create("Save",        Gtk::Stock::SAVE));
  action_group->add(Gtk::Action::create("Quit",        Gtk::Stock::QUIT),
                    sigc::mem_fun(*this, &EditorWindow::on_quit));

  action_group->add(Gtk::Action::create("MenuEdit",    "_Edit"));
  action_group->add(Gtk::Action::create("Cut",         Gtk::Stock::CUT));
  action_group->add(Gtk::Action::create("Copy",        Gtk::Stock::COPY));
  action_group->add(Gtk::Action::create("Paste",       Gtk::Stock::PASTE));

  action_group->add(Gtk::Action::create("MenuView",    "_View"));
  action_group->add(Gtk::Action::create("Zoom100",     Gtk::Stock::ZOOM_100));
  action_group->add(Gtk::Action::create("ZoomIn",      Gtk::Stock::ZOOM_IN));
  action_group->add(Gtk::Action::create("ZoomOut",     Gtk::Stock::ZOOM_OUT));

  action_group->add(Gtk::Action::create("Play",        Gtk::Stock::MEDIA_PLAY));

  action_group->add(Gtk::Action::create("MenuHelp",    "_Help"));
  action_group->add(Gtk::Action::create("About",       Gtk::Stock::ABOUT),
                    sigc::mem_fun(*this, &EditorWindow::on_about_clicked));

  //sigc::mem_fun(*this, &EditorWindow::on_action_file_new) );
  //sigc::mem_fun(*this, &EditorWindow::on_action_file_open) );
  //sigc::mem_fun(*this, &EditorWindow::on_action_file_quit) );

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string(ui_info);

  add_accel_group(ui_manager->get_accel_group());

  // Vbox
  vbox.pack_start(*ui_manager->get_widget("/MenuBar"), Gtk::PACK_SHRINK);
  vbox.pack_start(*ui_manager->get_widget("/ToolBar"), Gtk::PACK_SHRINK);
  vbox.add(hbox);
  vbox.add(status);

  // Hbox
  hbox.pack_start(*ui_manager->get_widget("/ToolBox"), Gtk::PACK_SHRINK);
  dynamic_cast<Gtk::Toolbar*>(ui_manager->get_widget("/ToolBox"))->set_orientation(Gtk::ORIENTATION_VERTICAL);
  hbox.add(hpaned);

  hpaned.add(text_view);
  hpaned.add(vpaned);
  vpaned.add1(object_tree);
  vpaned.add2(object_tree2);

  // Window
  add(vbox);

  text_view.set_size_request(800, 600);
  //object_tree.set_size_request(300, 500);

  // Show everything
  text_view.show();
  object_tree.show();
  object_tree2.show();
  status.show();

  hpaned.show();
  vpaned.show();

  hbox.show();
  vbox.show();
}

EditorWindow::~EditorWindow()
{
}

void
EditorWindow::on_about_clicked()
{
  AboutWindow about_dialog;
  about_dialog.run();
}

void
EditorWindow::on_quit()
{
  hide();
}

/* EOF */
