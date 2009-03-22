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
#include <gdkmm/pixbuf.h>

#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "sector_model.hpp"

ObjectTreeColumns* ObjectTreeColumns::instance_ = 0;

SectorModel::SectorModel()
{  
  objects_tree = Gtk::TreeStore::create(ObjectTreeColumns::instance());

  root_it = objects_tree->append();
  (*root_it)[ObjectTreeColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*root_it)[ObjectTreeColumns::instance().name]      = Glib::ustring("Scene");
  (*root_it)[ObjectTreeColumns::instance().visible]   = false;

  objects_tree->signal_row_changed().connect(sigc::mem_fun(*this, &SectorModel::on_row_changed));
  objects_tree->signal_row_deleted().connect(sigc::mem_fun(*this, &SectorModel::on_row_deleted));
  objects_tree->signal_row_has_child_toggled().connect(sigc::mem_fun(*this, &SectorModel::on_row_has_child_toggled));
  objects_tree->signal_row_inserted().connect(sigc::mem_fun(*this, &SectorModel::on_row_inserted));
  objects_tree->signal_rows_reordered().connect(sigc::mem_fun(*this, &SectorModel::on_rows_reordered));
}

void
SectorModel::add(const std::string& path, const Vector2f& pos)
{
  ObjectModelHandle obj = ObjectModel::create("obj:" + path, path, pos);

  objects.push_back(obj);

  Gtk::TreeStore::iterator it = objects_tree->append(root_it->children());
  (*it)[ObjectTreeColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[ObjectTreeColumns::instance().name]      = obj->get_name();
  (*it)[ObjectTreeColumns::instance().visible]   = false; 
}

void
SectorModel::draw(SceneContext& sc)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->draw(sc);
    }
}

void
SectorModel::on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  std::cout << "ObjectTree:on_row_changed" << std::endl;
}

void
SectorModel::on_row_deleted(const Gtk::TreeModel::Path& path)
{
  std::cout << "ObjectTree:on_row_deleted" << std::endl;
}

void
SectorModel::on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  std::cout << "ObjectTree:on_row_has_child_toggled" << std::endl;
}

void
SectorModel::on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  std::cout << "ObjectTree:on_row_inserted" << std::endl;
}

void
SectorModel::on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order)
{
  std::cout << "ObjectTree:on_row_reordered" << std::endl;
}

/* EOF */
