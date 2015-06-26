/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_NAVIGATION_NODE_HPP
#define HEADER_WINDSTILLE_NAVIGATION_NODE_HPP

#include <vector>

#include "math/vector2f.hpp"

#include "navigation/edge_position.hpp"

class Node
{
private:
  Vector2f pos;

public:
  /** Edges connected to this node */
  typedef std::vector<EdgePosition> Edges;
  Edges edges;

public:
  Node(const Vector2f& pos_);
  ~Node();

  Vector2f get_pos() const { return pos; }
  void     set_pos(const Vector2f& p) { pos = p; }

  /** Connect the given edge to the node, the position is used to
      mark the end of the edge that is actually connected */
  void add_edge(const EdgePosition& edge);

  void remove_edge(Edge* edge);

private:
  Node(const Node&);
  Node& operator=(const Node&);
};

#endif

/* EOF */
