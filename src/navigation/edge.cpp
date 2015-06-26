/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "navigation/edge.hpp"

#include <assert.h>

#include "navigation/node.hpp"

Edge::Edge(Node* node1_, Node* node2_, Properties props_) :
  node1(node1_),
  node2(node2_),
  properties(props_)
{
  node1->add_edge(EdgePosition(this, 0.0f));
  node2->add_edge(EdgePosition(this, 1.0f));
}

Edge::~Edge()
{
  node1->remove_edge(this);
  node2->remove_edge(this);
}

float
Edge::angle(Edge* /*seg*/)
{
  assert(!"Implement me");
  return 0.0f;
}

Line
Edge::get_line() const
{
  return Line(node1->get_pos(),
              node2->get_pos());
}

Vector2f
Edge::get_vector() const
{
  return node2->get_pos() - node1->get_pos();
}

/* EOF */
