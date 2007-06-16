/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_NAVIGATION_GRAPH_HPP
#define HEADER_NAVIGATION_GRAPH_HPP

#include <vector>
#include "math/line.hpp"

class Node;
class Segment;

/** */
class NavigationGraph
{
private:
  typedef std::vector<Node*>    Nodes;
  typedef std::vector<Segment*> Segments;
  
  Nodes    nodes;
  Segments segments;
  
  // insert some spartial thingy here

public:
  NavigationGraph();
  ~NavigationGraph();

  Node*    add_node(const Vector& pos);
  Segment* add_segment(Node* node1, Node* node2);

  void remove_node(Node* node);
  void remove_segment(Segment* segment);

  void split_segment(Segment* segment);

  /** Find segments that collide with the given line */
  std::vector<Segment*> find_segments(const Line& line);

  /** Find nodes that are near the given point */
  std::vector<Node*> find_nodes(const Vector& pos, float radius);

  /** Find the closest node */
  Node* find_closest_node(const Vector& pos, float radius);

  Segment* find_closest_segment(const Vector& pos, float radius);

  /** Find segments that are near the given point */
  std::vector<Segment*> find_segments(const Vector& pos, float radius);

  /** Draw the navigation graph, for debugging only */
  void draw();
private:
  NavigationGraph (const NavigationGraph&);
  NavigationGraph& operator= (const NavigationGraph&);
};

#endif

/* EOF */
