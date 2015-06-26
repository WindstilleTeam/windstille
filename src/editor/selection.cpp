/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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
#include <boost/bind.hpp>

#include "editor/windstille_widget.hpp"
#include "editor/editor_window.hpp"
#include "editor/selection.hpp"
#include "util/log.hpp"

Selection::Selection() :
  objects(),
  object_orig_pos(),
  non_moveable_objects(),
  moving(false),
  signal_changed()
{
}

Selection::~Selection()
{
}

void
Selection::add(const ObjectModelHandle& object)
{
  if (!object)
  {
    std::cout << "Error: Selection:add(): trying to add NULL pointer" << std::endl;
  }
  else
  {
    objects.push_back(object);
    signal_changed();
  }
}

void
Selection::remove(const ObjectModelHandle& object)
{
  Objects::iterator it = std::find(objects.begin(), objects.end(), object);
  if (it != objects.end())
  {
    objects.erase(it);
    signal_changed();
  }
  else
  {
    EditorWindow::current()->print("Selection:remove(): object not in selection");
  }
}

bool
Selection::empty() const
{
  return objects.empty();
}

void
Selection::clear()
{
  objects.clear();
  signal_changed();
}

bool
Selection::has_object(ObjectModelHandle object) const
{
  Objects::const_iterator it = std::find(objects.begin(), objects.end(), object);
  return it != objects.end();
}

bool
Selection::contains_parent(ObjectModelHandle object)
{
  ObjectModelHandle parent = object->get_parent();
  while (parent)
  {
    if (has_object(parent))
      return true;

    parent = parent->get_parent();
  }

  return false;
}

void
Selection::on_move_start()
{
  log_debug(this << " objects: " << objects.size() << " object_orig_pos: " << object_orig_pos.size());

  moving = true;

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if (contains_parent(*i))
    {
      non_moveable_objects.insert(*i);
    }
  }

  object_orig_pos.clear();

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if (non_moveable_objects.find(*i) == non_moveable_objects.end())
    {
      //(*i)->on_move_start();
      object_orig_pos.push_back((*i)->get_rel_pos());
    }
  }

  signal_changed();
}

void
Selection::on_move_update(const Vector2f& offset)
{
  log_debug(this << " objects: " << objects.size() << " object_orig_pos: " << object_orig_pos.size());

  if (!object_orig_pos.empty())
  {
    for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (non_moveable_objects.find(*i) == non_moveable_objects.end())
      {
        (*i)->set_rel_pos(object_orig_pos[i - objects.begin()] + offset);
      }
    }
  }
}

void
Selection::on_move_end(WindstilleWidget& wst, const Vector2f& offset)
{
  log_debug(this << " objects: " << objects.size() << " object_orig_pos: " << object_orig_pos.size());

  moving = false;

  wst.get_document().undo_group_begin();
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if (non_moveable_objects.find(*i) == non_moveable_objects.end())
    {
      wst.get_document().execute(boost::bind(&ObjectModel::set_rel_pos, *i, object_orig_pos[i - objects.begin()]),
                                 boost::bind(&ObjectModel::set_rel_pos, *i, object_orig_pos[i - objects.begin()] + offset));
    }
  }
  wst.get_document().undo_group_end();

  non_moveable_objects.clear();
  signal_changed();
}

SelectionHandle
Selection::clone() const
{
  SelectionHandle selection = Selection::create();
  std::map<ObjectModelHandle, ObjectModelHandle> parent_map;
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i)
  {
    ObjectModelHandle obj = (*i)->clone();
    parent_map[*i] = obj;
    selection->add(obj);
  }

  // Second pass to set the parents to the cloned objects
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
  {
    if ((*i)->get_parent())
    {
      std::map<ObjectModelHandle, ObjectModelHandle>::iterator it = parent_map.find((*i)->get_parent());

      if (it == parent_map.end())
      {
        // When the parent wasn't part of the selection, leave
        // the parent untouched
      }
      else
      {
        (*i)->set_parent(it->second);
      }
    }
  }

  return selection;
}

Rectf
Selection::get_bounding_box() const
{
  if (empty())
  {
    return Rectf();
  }
  else
  {
    Rectf rect = objects.front()->get_bounding_box();
    for(Objects::const_iterator i = objects.begin()+1; i != objects.end(); ++i)
    {
      rect = rect.grow((*i)->get_bounding_box());
    }
    return rect;
  }
}

void
Selection::reset()
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i)
  {
    (*i)->reset();
  }
}

void
Selection::add_control_points(std::vector<ControlPointHandle>& control_points)
{
  if (!empty())
  {
    if (size() == 1)
    {
      objects[0]->add_control_points(control_points);
    }
    else
    {
      const Rectf& rect = get_bounding_box();
      control_points.push_back(ControlPoint::create(Vector2f(rect.left, rect.top)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.right, rect.top)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.left, rect.bottom)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.right, rect.bottom)));
    }
  }
}

/* EOF */
