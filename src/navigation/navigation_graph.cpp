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

#include <iostream>
#include <algorithm>
#include "display/display.hpp"
#include "node.hpp"
#include "segment.hpp"
#include "navigation_graph.hpp"

NavigationGraph::NavigationGraph()
{
  
}

NavigationGraph::~NavigationGraph()
{
}

Node*
NavigationGraph::add_node(const Vector& pos)
{
  Node* node = new Node(pos);
  nodes.push_back(node);
  return node;
}

void
NavigationGraph::remove_segment(Segment* segment)
{
  // FIXME: Slow
  Segments::iterator i = std::find(segments.begin(), segments.end(), segment);
  if (i != segments.end())
    {
      segments.erase(i);
    }

  delete segment;
}

struct ContainsNode
{
  Node* node;

  ContainsNode(Node* node_) : node(node_) {}

  bool operator()(Segment* segment) {
    return 
      (segment->get_node1() == node ||
       segment->get_node2() == node);
  }
};

void
NavigationGraph::remove_node(Node* node)
{
  // FIXME: Slow

  // Remove all segments that would get invalid by removing the node
  Segments::iterator i = std::remove_if(segments.begin(), segments.end(), ContainsNode(node));
  if (i != segments.end())
    {
      segments.erase(i, segments.end());
    }
  
  // Remove the node itself 
  Nodes::iterator j = std::find(nodes.begin(), nodes.end(), node);
  if (j != nodes.end())
    {
      nodes.erase(j);
    }  

  delete node;
}

Segment*
NavigationGraph::add_segment(Node* node1, Node* node2)
{
  Segment* segment = new Segment(node1, node2);
  segments.push_back(segment);
  return segment;
}

void
NavigationGraph::split_segment(Segment* segment)
{
  Node* node1 = segment->get_node1();
  Node* node3 = segment->get_node2();
  Node* node2 = add_node(0.5f * (node1->get_pos() + node3->get_pos()));

  remove_segment(segment);
  add_segment(node1, node2);  
  add_segment(node2, node3);  
}

std::vector<Segment*>
NavigationGraph::find_segments(const Line& line)
{
  return std::vector<Segment*>();
}

std::vector<Node*>
NavigationGraph::find_nodes(const Vector& pos, float radius)
{
  return std::vector<Node*>();
}

std::vector<Segment*>
NavigationGraph::find_segments(const Vector& pos, float radius)
{
  return std::vector<Segment*>();
}

Node*
NavigationGraph::find_closest_node(const Vector& pos, float radius)
{
  // FIXME: Optimize this with spatial tree thingy
  Node* node = 0;
  float min_distance = radius;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    {
      float current_distance = (pos - (*i)->get_pos()).length();
      if (current_distance < min_distance)
        {
          min_distance = current_distance;
          node = *i;
        }
    }
  
  return node;
}

Segment*
NavigationGraph::find_closest_segment(const Vector& pos, float radius)
{
  Segment* segment   = 0;
  float min_distance = radius;

  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      float current_distance = Line((*i)->get_node1()->get_pos(),
                                    (*i)->get_node2()->get_pos()).distance(pos);
      if (current_distance < min_distance)
        {
          min_distance = current_distance;
          segment = *i;
        }
    }

  return segment;
}

void
NavigationGraph::draw()
{
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      Display::draw_segment(Line((*i)->get_node1()->get_pos(),
                                 (*i)->get_node2()->get_pos()),
                            Color(1.0f, 0.0f, 0.0f));
    }

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    {
      Display::fill_rect(Rectf((*i)->get_pos() - Vector(4,4), Sizef(9, 9)),
                         Color(1.0f, 1.0f, 0.0f));
    }
}

/* EOF */
