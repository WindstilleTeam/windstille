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

#include "editor/layer.hpp"

#include "editor/layer_manager_columns.hpp"
#include "editor/sector_model.hpp"

Layer::Layer(SectorModel& sector)
  : m_sector(sector),
    objects(),
    name(),
    visible(true),
    locked(false)
{
}

Layer::~Layer()
{
}

bool
Layer::has_object(const ObjectModelHandle& object) const
{
  return std::find(objects.begin(), objects.end(), object) != objects.end();
}

void
Layer::add(const ObjectModelHandle& object)
{
  objects.push_back(object);
}

void
Layer::remove(const ObjectModelHandle& object)
{
  objects.remove(object);
}

Layer::iterator
Layer::erase(iterator it)
{
  it = objects.erase(it);
  return it;
}

void
Layer::draw(SceneContext& sc, const SelectMask& select_mask)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()))
      (*i)->draw(sc);
  }
}

void
Layer::sync(const Gtk::TreeModel::Row& row)
{
  name    = ((Glib::ustring)row[LayerManagerColumns::instance().name]).raw();
  visible = row[LayerManagerColumns::instance().visible];
  locked  = row[LayerManagerColumns::instance().locked];
}

void
Layer::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    (*i)->update(delta);
  }
}

ObjectModelHandle
Layer::get_object_at(const Vector2f& pos, const SelectMask& select_mask) const
{
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        (*i)->is_at(pos))
    {
      return *i;
    }
  }
  return ObjectModelHandle();
}

SelectionHandle
Layer::get_selection(const Rectf& rect, const SelectMask& select_mask) const
{
  SelectionHandle selection = Selection::create();

  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        rect.contains((*i)->get_bounding_box()))
    {
      selection->add(*i);
    }
  }

  return selection;
}

void
Layer::raise_to_top(ObjectModelHandle object)
{
  objects.remove(object);
  objects.push_back(object); 
}

void
Layer::lower_to_bottom(ObjectModelHandle object)
{
  objects.remove(object);
  objects.push_front(object); 
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
Layer::raise(ObjectModelHandle object)
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
Layer::lower(ObjectModelHandle object)
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

SnapData
Layer::snap_object(ObjectModelHandle object, const std::set<ObjectModelHandle>& ignore_objects) const
{
  SnapData best_snap;

  // Find the smallest snap offset
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
  {
    // object is not in the list of objects to ignore
    if ((*i)->is_snappable() &&
        ignore_objects.find(*i) == ignore_objects.end())
    {
      SnapData snap = object->snap_to_object((*i)->get_bounding_box());
      best_snap.merge(snap);
    }
  }

  return best_snap;
}

void
Layer::write(FileWriter& writer) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i)
  {
    (*i)->write(writer);
  }
}

/* EOF */
