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

#ifndef HEADER_WINDSTILLE_EDITOR_EDITOR_WINDOW_HPP
#define HEADER_WINDSTILLE_EDITOR_EDITOR_WINDOW_HPP

#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/textview.h>
#include <gtkmm/paned.h>
#include <gtkmm/window.h>

#include "object_tree.hpp"

class EditorWindow : public Gtk::Window
{
private:
  Gtk::VBox   vbox;
  Gtk::HBox   hbox;
  Gtk::HPaned hpaned;
  Gtk::VPaned vpaned;
  Gtk::Statusbar status;
  
  Gtk::TextView text_view;
  ObjectTree    object_tree;
  ObjectTree    object_tree2;

  Glib::RefPtr<Gtk::UIManager>   ui_manager;
  Glib::RefPtr<Gtk::ActionGroup> action_group;

public:
  EditorWindow();
  virtual ~EditorWindow();

protected:
  void on_about_clicked();
  void on_quit();

private:
  EditorWindow(const EditorWindow&);
  EditorWindow& operator=(const EditorWindow&);
};

#endif

/* EOF */
