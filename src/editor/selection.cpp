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
#include "editor_window.hpp"
#include "selection.hpp"

Selection::Selection()
{
}

Selection::~Selection()
{
}

void
Selection::add(const ObjectModelHandle& object)
{
  objects.push_back(object);
}

void
Selection::remove(const ObjectModelHandle& object)
{
  Objects::iterator it = std::find(objects.begin(), objects.end(), object);
  if (it != objects.end())
    {
      objects.erase(it);
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

bool
Selection::has_object(ObjectModelHandle object) const
{
  Objects::const_iterator it = std::find(objects.begin(), objects.end(), object);
  return it != objects.end();
}

void
Selection::on_move_start()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->on_move_start();
    }
}

void
Selection::on_move_update(const Vector2f& offset)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->on_move_update(offset);
    }
}

void
Selection::on_move_end(const Vector2f& offset)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->on_move_end(offset);
    }
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

/* EOF */
