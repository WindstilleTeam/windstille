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

Selection::iterator
Selection::begin()
{
  return objects.begin();
}

Selection::iterator
Selection::end()
{
  return objects.end();
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

/* EOF */
