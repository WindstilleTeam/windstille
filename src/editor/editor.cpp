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

#include <gtkglmm.h>
#include <gtkmm.h>
#include <gtkmm/gl/widget.h>
#include <libglademm/xml.h>
#include <iostream>

class MyModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<bool>          visible;
  Gtk::TreeModelColumn<Glib::ustring> filename;
  Gtk::TreeModelColumn<Glib::ustring> description;

  MyModelColumns() { add(filename); add(visible); add(description); }
};

void create_tree(Gtk::TreeView& treeview)
{
  MyModelColumns m_columns;
  Glib::RefPtr<Gtk::TreeStore> list_store = Gtk::TreeStore::create(m_columns);

  Gtk::TreeStore::iterator root = list_store->append();
  (*root)[m_columns.visible]     = false;
  (*root)[m_columns.filename]    = Glib::ustring("Hello World");
  (*root)[m_columns.description] = Glib::ustring("Hello World");

  for(int i = 0; i < 10; ++i)
    {
      Gtk::TreeStore::iterator it  = list_store->append(root->children());
      (*it)[m_columns.visible]     = false;
      (*it)[m_columns.filename]    = Glib::ustring("Hello World");
      (*it)[m_columns.description] = Glib::ustring("Hello World");

      Gtk::TreeModel::iterator it2  = list_store->append(it->children());
      (*it)[m_columns.visible]      = true;
      (*it2)[m_columns.filename]    = Glib::ustring("Hello World");
      (*it2)[m_columns.description] = Glib::ustring("Hello World");
    }

  treeview.set_headers_clickable();
  //treeview.set_headers_visible(false);
  //treeview.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_BOTH);
  treeview.set_enable_tree_lines();
  treeview.set_model(list_store);


  treeview.append_column("Name", m_columns.filename);
  //treeview.append_column("Blaca", m_columns.description);
  treeview.append_column_editable("Visible", m_columns.visible);

  treeview.expand_all();

  treeview.show();
}

int main(int argc, char** argv)
{
  try 
    {
      Gtk::Main kit(&argc, &argv);
      Gtk::GL::init(&argc, &argv);

      Glib::RefPtr<Gnome::Glade::Xml> xml = Gnome::Glade::Xml::create("windstille.glade");

      Gtk::TreeView& treeview = dynamic_cast<Gtk::TreeView&>(*xml->get_widget("treeview1"));

      create_tree(treeview);
      
      Gtk::Window& window = dynamic_cast<Gtk::Window&>(*xml->get_widget("window1"));
      // Gtk::ScrolledWindow& hbox = dynamic_cast<Gtk::ScrolledWindow&>(*xml->get_widget("scrolledwindow1"));
      window.show();

      Gtk::Main::run(window);
    }
  catch(std::exception& err)
    {
      std::cout << "Exception: " << err.what() << std::endl;
    }
  catch(...)
    {
      std::cout << "Unknown exception catched" << std::endl;
    }

  return 0;
}

/* EOF */
