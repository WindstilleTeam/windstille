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

#include <limits>
#include <assert.h>
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
SectorModel::add(const ObjectModelHandle& object)  
{
  objects.push_back(object);

  Gtk::TreeStore::iterator it = objects_tree->append(root_it->children());
  (*it)[ObjectTreeColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[ObjectTreeColumns::instance().name]      = object->get_name();
  (*it)[ObjectTreeColumns::instance().visible]   = false; 
}

void
SectorModel::remove(const ObjectModelHandle& object)
{
  objects.remove(object);
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
SectorModel::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->update(delta);
    }
}

ObjectModelHandle
SectorModel::get_object_at(const Vector2f& pos) const
{
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      if ((*i)->get_bounding_box().is_inside(pos))
        {
          return *i;
        }
    }
  return ObjectModelHandle();
}

SelectionHandle
SectorModel::get_selection(const Rectf& rect) const
{
  SelectionHandle selection = Selection::create();

  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      if ((*i)->get_bounding_box().is_overlapped(rect))
        {
          selection->add(*i);
        }
    }

  return selection;
}

struct OverlapsWith
{
  Rectf rect;

  OverlapsWith(const Rectf& rect_)
    : rect(rect_)
  {}

  bool operator()(const ObjectModelHandle& object) {
    return rect.is_overlapped(object->get_bounding_box());
  }
};

void
SectorModel::raise(ObjectModelHandle object)
{
  Objects::iterator i = std::find(objects.begin(), objects.end(), object);
  assert(i != objects.end());
  Objects::iterator j = i;
  ++j;
  j = std::find_if(j, objects.end(), OverlapsWith(object->get_bounding_box()));

  if (j == objects.end())
    {
      // object overlaps with no other object, no point in raising it
    }
  else
    {
      objects.erase(i);
      objects.insert(++j, object);
    }
}

void
SectorModel::lower(ObjectModelHandle object)
{
  // Mostly the same as raise, just with reverse iterators
  Objects::reverse_iterator i = std::find(objects.rbegin(), objects.rend(), object);

  Objects::reverse_iterator j = i;
  ++j;
  j = std::find_if(j, objects.rend(), OverlapsWith(object->get_bounding_box()));

  if (j == objects.rend())
    {
      // object overlaps with no other object, no point in lowering it
    }
  else
    {
      // the base() of base in one further then where the reverse
      // iterator was, so we have to move back to get the same
      // position
      objects.erase(--(i.base()));
      objects.insert(--(j.base()), object);
    }
}

void
SectorModel::raise_to_top(ObjectModelHandle object)
{
  objects.remove(object);
  objects.push_back(object); 
}

void
SectorModel::lower_to_bottom(ObjectModelHandle object)
{
  objects.remove(object);
  objects.push_front(object); 
}

SnapData
SectorModel::snap_object(const Rectf& rect) const
{
  float min_x_offset = std::numeric_limits<float>::max();
  float min_y_offset = std::numeric_limits<float>::max();
  SnapData best_snap;

  // Find the smallest snap offset
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      SnapData snap = (*i)->snap_object(rect);

      if (snap.x_set)
        {
          if (snap.offset.x < min_x_offset)
            {
              min_x_offset = snap.offset.x;
              best_snap.offset.x = snap.offset.x;
              best_snap.x_set = true;
            }
        }

      if (snap.y_set)
        {
          if (snap.offset.y < min_y_offset)
            {
              min_y_offset = snap.offset.y;
              best_snap.offset.y = snap.offset.y;
              best_snap.y_set = true;
            }
        }
    }

  return best_snap;
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
