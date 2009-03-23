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

#ifndef HEADER_WINDSTILLE_EDITOR_SELECTION_HPP
#define HEADER_WINDSTILLE_EDITOR_SELECTION_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

#include "object_model.hpp"

class Selection;
typedef boost::shared_ptr<Selection> SelectionHandle;

class Selection
{
private:
  typedef std::vector<ObjectModelHandle> Objects;
  Objects objects;

public:
  typedef Objects::iterator iterator;
  typedef Objects::reverse_iterator reverse_iterator;

public:
  static SelectionHandle create() { return SelectionHandle(new Selection()); }

  Selection();
  ~Selection();

  void add(const ObjectModelHandle& object);
  
  Selection::iterator begin() { return objects.begin(); }
  Selection::iterator end()   { return objects.end(); }

  Selection::reverse_iterator rbegin() { return objects.rbegin(); }
  Selection::reverse_iterator rend() { return objects.rend(); }

  bool empty() const;

  bool has_object(ObjectModelHandle object) const;

  void on_move_start();
  void on_move_update(const Vector2f& offset);
  void on_move_end(const Vector2f& offset);
  
private:
  Selection(const Selection&);
  Selection& operator=(const Selection&);
};

#endif

/* EOF */
