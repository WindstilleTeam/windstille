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

#include <gdkmm/pixbuf.h>
#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "sector_model.hpp"

class ObjectTreeColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > type_icon;
  Gtk::TreeModelColumn<Glib::ustring>              name;
  Gtk::TreeModelColumn<bool>                       visible;

  ObjectTreeColumns() {
    add(type_icon); 
    add(name); 
    add(visible); 
  }
};

SectorModel::SectorModel()
{
  ObjectTreeColumns columns;
  objects_tree = Gtk::TreeStore::create(columns);
}

void
SectorModel::add(const Vector2f& obj)
{
  objects.push_back(obj);
}

void
SectorModel::draw(SceneContext& sc)
{
  Surface surface("images/hedgehog.png");

  for(std::vector<Vector2f>::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      sc.color().draw(surface, i->x, i->y);
      sc.control().fill_rect(Rectf(*i - Vector2f(8, 8), Sizef(16, 16)), Color(1,0,0));
    }
}

/* EOF */
