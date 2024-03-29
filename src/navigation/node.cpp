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

#include "navigation/node.hpp"

namespace windstille {


Node::Node(glm::vec2 const& pos_) :
  pos(pos_),
  edges()
  // FIXME: Do something with id
{
}


Node::~Node()
{
}


void
Node::add_edge(EdgePosition const& position)
{
  edges.push_back(position);
}


void
Node::remove_edge(Edge* edge)
{
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    if (i->edge == edge)
    {
      edges.erase(i);
      return;
    }
  }
}


} // namespace windstille

/* EOF */
