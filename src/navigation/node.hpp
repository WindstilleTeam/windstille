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

#include <glm/glm.hpp>

#include "navigation/edge_position.hpp"

namespace windstille {

class Node
{
private:
  glm::vec2 pos;

public:
  /** Edges connected to this node */
  typedef std::vector<EdgePosition> Edges;
  Edges edges;

public:
  Node(glm::vec2 const& pos_);
  ~Node();

  glm::vec2 get_pos() const { return pos; }
  void     set_pos(glm::vec2 const& p) { pos = p; }

  /** Connect the given edge to the node, the position is used to
      mark the end of the edge that is actually connected */
  void add_edge(EdgePosition const& edge);

  void remove_edge(Edge* edge);

private:
  Node(Node const&);
  Node& operator=(Node const&);
};

} // namespace windstille

#endif

/* EOF */
