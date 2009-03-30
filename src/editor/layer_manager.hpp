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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_MANAGER_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_MANAGER_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/actiongroup.h>

class SectorModel;
class EditorWindow;

class LayerManager : public Gtk::VBox
{
private:
  EditorWindow& editor;
  Gtk::Label label;
  Gtk::ScrolledWindow scrolled;
  Gtk::TreeView treeview;

  Glib::RefPtr<Gtk::ToggleAction> auto_lock;

public:
  LayerManager(EditorWindow& editor);
  virtual ~LayerManager();

  void set_model(SectorModel* model);

  void on_cursor_changed();
  void on_columns_changed();

  Gtk::TreeView& get_treeview() { return treeview; }

  void on_auto_lock(Glib::RefPtr<Gtk::ToggleAction> auto_lock);

private:
  LayerManager(const LayerManager&);
  LayerManager& operator=(const LayerManager&);
};

#endif

/* EOF */
