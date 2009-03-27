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

#include "hard_layer.hpp"

HardLayer::HardLayer()
{
}

HardLayer::~HardLayer()
{
}

void
HardLayer::add(const ObjectModelHandle& object)
{
  objects.push_back(object);
}

void
HardLayer::remove(const ObjectModelHandle& object)
{
  objects.remove(object);
}

void
HardLayer::draw(SceneContext& sc, const Layers& layers)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (layers.match((*i)->get_layers()))
        (*i)->draw(sc);
    }
}

void
HardLayer::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->update(delta);
    }
}

ObjectModelHandle
HardLayer::get_object_at(const Vector2f& pos, const Layers& layers) const
{
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      if (layers.match((*i)->get_layers()) &&
          (*i)->get_bounding_box().is_inside(pos))
        {
          return *i;
        }
    }
  return ObjectModelHandle();
}

SelectionHandle
HardLayer::get_selection(const Rectf& rect, const Layers& layers) const
{
 SelectionHandle selection = Selection::create();

  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      if (layers.match((*i)->get_layers()) &&
          (*i)->get_bounding_box().is_overlapped(rect))
        {
          selection->add(*i);
        }
    }

  return selection;
}

void
HardLayer::raise_to_top(ObjectModelHandle object)
{
  objects.remove(object);
  objects.push_back(object); 
}

void
HardLayer::lower_to_bottom(ObjectModelHandle object)
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
HardLayer::raise(ObjectModelHandle object)
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
HardLayer::lower(ObjectModelHandle object)
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
HardLayer::snap_object(const Rectf& rect, const std::set<ObjectModelHandle>& ignore_objects) const
{
  //float min_x_offset = std::numeric_limits<float>::max();
  //float min_y_offset = std::numeric_limits<float>::max();
  SnapData best_snap;

  // Find the smallest snap offset
  for(Objects::const_reverse_iterator i = objects.rbegin(); i != objects.rend(); ++i)
    {
      // object is not in the list of objects to ignore
      if ((*i)->is_snappable() &&
          ignore_objects.find(*i) == ignore_objects.end())
        {
          SnapData snap = (*i)->snap_object(rect);
          best_snap.merge(snap);
        }
    }

  return best_snap;
}

void
HardLayer::write(FileWriter& writer) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->write(writer);
    }
}

HardLayerHandle
HardLayer::clone() const
{
  return HardLayerHandle();
}

/* EOF */
