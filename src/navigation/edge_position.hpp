/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_NAVIGATION_EDGE_POSITION_HPP
#define HEADER_WINDSTILLE_NAVIGATION_EDGE_POSITION_HPP

#include "math/vector2f.hpp"

class Edge;
class Node;

class EdgePosition
{
public:
  Edge* edge;

  /** Position on the edge, stored with range [0,1], not
      world-co */
  float pos;

public:
  EdgePosition();
  EdgePosition(Edge* edge_, float pos_);

  void set_pos(Edge* edge_, float pos_);

  /** Move forward \a adv of units in world-co, when a node is hit,
   *  the function returns and let the user decide how to continue
   *
   *  @param[in,out] adv the amount of advancment to be done, the
   *                     amount of units that wheren't use on the
   *                     given edge
   *
   *  @param[out] next_node if the advance ends at a node, it gets
   *                        returned in next_node
   */
  void advance(float& adv, Node*& next_node);

  /** Move forward \a adv of units in world-co, when a node is hit,
   *  the function returns and let the user decide how to
   *  continue. \a adv is projected onto the current edge to figure
   *  out how far we should go
   *
   * @param[in,out] adv the amount of advancment to be done, the
   *                    amount of units that wheren't use on the given
   *                    edge
   *
   * @param[out] next_node if the advance ends at a node, it gets
   *                       returned in next_node
   */
  void advance(Vector2f& adv, Node*& next_node);

  Edge* get_edge() const { return edge; }
  float get_float_pos() const { return pos; }

  Vector2f get_pos() const;
};

#endif

/* EOF */
