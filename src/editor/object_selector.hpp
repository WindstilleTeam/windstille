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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_SELECTOR_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_SELECTOR_HPP

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gdkmm/dragcontext.h>
#include <gtkmm/iconview.h>

class EditorWindow;

class ObjectSelector : public Gtk::VBox
{
private:
  EditorWindow& editor;

  Gtk::Label label;
  Gtk::ScrolledWindow scrolled;
  Gtk::IconView iconview;
  Glib::RefPtr<Gtk::ListStore> list_store;
  
public:
  ObjectSelector(EditorWindow& editor);
  virtual ~ObjectSelector();

  void add_object(const std::string& pathname,
                  const Glib::RefPtr<Gdk::Pixbuf>& icon);

  void refresh();

protected:
  void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
  void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context,
                        Gtk::SelectionData& selection_data, 
                        guint info, guint time);
  
private:
  ObjectSelector(const ObjectSelector&);
  ObjectSelector& operator=(const ObjectSelector&);
};

#endif

/* EOF */
