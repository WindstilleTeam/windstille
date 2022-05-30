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

#ifndef HEADER_WINDSTILLE_EDITOR_NODE_SELECTION_HPP
#define HEADER_WINDSTILLE_EDITOR_NODE_SELECTION_HPP

#include <set>

#include "navigation/navigation_graph.hpp"

class NodeSelection
{
private:
  std::set<NodeHandle> nodes;

public:
  typedef std::set<NodeHandle>::iterator iterator;

  NodeSelection();
  ~NodeSelection();

  iterator begin() { return nodes.begin(); }
  iterator end()   { return nodes.end(); }

private:
  NodeSelection(NodeSelection const&);
  NodeSelection& operator=(NodeSelection const&);
};

#endif

/* EOF */
