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

#ifndef HEADER_WINDSTILLE_EDITOR_SELECTION_HPP
#define HEADER_WINDSTILLE_EDITOR_SELECTION_HPP

#include <vector>
#include <set>
#include <memory>

#include "editor/control_point.hpp"
#include "editor/object_model.hpp"

class Selection;
class WindstilleWidget;
typedef std::shared_ptr<Selection> SelectionHandle;

class Selection
{
private:
  typedef std::vector<ObjectModelHandle> Objects;
  Objects objects;

  std::vector<glm::vec2> object_orig_pos;

  std::set<ObjectModelHandle> non_moveable_objects;

  bool moving;

  bool contains_parent(ObjectModelHandle object);

public:
  typedef Objects::iterator iterator;
  typedef Objects::reverse_iterator reverse_iterator;

public:
  static SelectionHandle create() { return SelectionHandle(new Selection()); }

  Selection();
  ~Selection();

  template<class Iter>
  void add(Iter begin_, Iter end_) {
    for(Iter i = begin_; i != end_; ++i)
    {
      add(*i);
    }
    signal_changed();
  }

  void add(ObjectModelHandle const& object);
  void remove(ObjectModelHandle const& object);

  int size() const { return static_cast<int>(objects.size()); }

  Selection::iterator begin() { return objects.begin(); }
  Selection::iterator end()   { return objects.end(); }

  Selection::reverse_iterator rbegin() { return objects.rbegin(); }
  Selection::reverse_iterator rend()   { return objects.rend(); }

  bool empty() const;
  void clear();

  bool is_moving() const { return moving; }

  bool has_object(ObjectModelHandle object) const;

  void on_move_start();
  void on_move_update(glm::vec2 const& offset);
  void on_move_end(WindstilleWidget& wst, glm::vec2 const& offset);

  /** Performs a deep clone of the selection */
  SelectionHandle clone() const;

  void add_control_points(std::vector<ControlPointHandle>& control_points);

  geom::frect get_bounding_box() const;

  void reset();

  sigc::signal<void> signal_changed;

private:
  Selection(Selection const&);
  Selection& operator=(Selection const&);
};

#endif

/* EOF */
