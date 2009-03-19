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

#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include "object_selector.hpp"

class ObjectIconColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<Glib::ustring> pathname;
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> >   icon;
  
  ObjectIconColumns() {
    add(pathname); 
    add(icon);
  }
};

ObjectSelector::ObjectSelector()
  : label("Object Selector", Gtk::ALIGN_LEFT)
{
  ObjectIconColumns columns;
  Glib::RefPtr<Gtk::ListStore> list_store = Gtk::ListStore::create(columns);

  Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file("icon.png");

  iconview.modify_base(Gtk::STATE_NORMAL, Gdk::Color("grey"));
  
  for(int i = 0; i < 10; ++i)
    {
      Gtk::ListStore::iterator it  = list_store->append();
      (*it)[columns.pathname] = Glib::ustring("Hello World");
      (*it)[columns.icon]     = icon;
    }
  iconview.set_model(list_store);

  iconview.set_pixbuf_column(columns.icon);
  //iconview.set_text_column(columns.pathname);

  iconview.set_model(list_store);

  std::vector<Gtk::TargetEntry> targets;
  targets.push_back(Gtk::TargetEntry("WindstilleObject"));
  iconview.drag_source_set(targets, Gdk::BUTTON1_MASK, Gdk::ACTION_COPY);
  iconview.signal_drag_begin().connect(sigc::mem_fun(*this, &ObjectSelector::on_drag_begin));

  scrolled.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

  scrolled.add(iconview);
  pack_start(label, Gtk::PACK_SHRINK);
  add(scrolled);
  
  show_all();
}

ObjectSelector::~ObjectSelector()
{

}

void
ObjectSelector::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
  context->set_icon(Gdk::Pixbuf::create_from_file("icon.png"), 0, 0);
}

/* EOF */
