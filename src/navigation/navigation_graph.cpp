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
#include <map>
#include <algorithm>
#include "display/display.hpp"
#include "node.hpp"
#include "segment.hpp"
#include "segment_position.hpp"
#include "navigation_graph.hpp"

NavigationGraph::NavigationGraph()
{
  
}

NavigationGraph::~NavigationGraph()
{
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    delete *i;

  for (Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    delete *i;
}

NodeHandle
NavigationGraph::add_node(const Vector& pos)
{
  Node* node = new Node(pos);
  nodes.push_back(node);
  return NodeHandle(node);
}

void
NavigationGraph::remove_segment(SegmentHandle segment)
{
  // FIXME: Slow
  Segments::iterator i = std::find(segments.begin(), segments.end(), segment.get());
  if (i != segments.end())
    {
      segments.erase(i);
      delete segment.get();
    }

  // FIXME: Throw exception here
}

void
NavigationGraph::remove_node(NodeHandle node)
{
  // FIXME: Slow

  // Remove all segments that would get invalid by removing the node
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      if ((*i)->get_node1() == node.get() ||
          (*i)->get_node2() == node.get())
        {
          delete *i;
          *i = 0;
        }
    }

  Segments::iterator new_end = std::remove(segments.begin(), segments.end(), (Segment*)0);
  if (new_end != segments.end())
    { 
      segments.erase(new_end, segments.end());
    }
  
  // Remove the node itself 
  Nodes::iterator j = std::find(nodes.begin(), nodes.end(), node.get());
  if (j != nodes.end())
    {
      nodes.erase(j);
      delete node.get();
    }  
}

SegmentHandle
NavigationGraph::add_segment(NodeHandle node1, NodeHandle node2)
{
  if (node1.get() != node2.get()) // node links to themself are forbidden
    { // FIXME: Find a way to figure out if the given segment already exists
      Segment* segment = new Segment(node1.get(), node2.get());
      segments.push_back(segment);
      return SegmentHandle(segment);
    }
  else
    {
      return SegmentHandle();
    }
}

void
NavigationGraph::split_segment(SegmentHandle segment)
{
  NodeHandle node1 = NodeHandle(segment->get_node1());
  NodeHandle node3 = NodeHandle(segment->get_node2());
  NodeHandle node2 = add_node(0.5f * (node1->get_pos() + node3->get_pos()));

  remove_segment(segment);
  add_segment(node1, node2);  
  add_segment(node2, node3);  
}

std::vector<SegmentPosition>
NavigationGraph::find_intersections(const Line& line)
{
  // FIXME: we might want to only return the first intersection, not
  // all of them or alternativly return ua
  std::vector<SegmentPosition> ret;
 
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      Line seg_line((*i)->get_node1()->get_pos(),
                    (*i)->get_node2()->get_pos());
      
      float ua, ub;
      if (line.intersect(seg_line, ua, ub))
        {
          ret.push_back(SegmentPosition(*i, ub));
        }
    }

  return ret;
}

std::vector<NodeHandle>
NavigationGraph::find_nodes(const Vector& pos, float radius)
{
  // FIXME: Optimize this with spatial tree thingy
  std::vector<NodeHandle> ret;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    {
      float distance = (pos - (*i)->get_pos()).length();
      if (distance < radius)
        {
          ret.push_back(NodeHandle(*i));
        }
    }
  
  return ret;
}

std::vector<SegmentHandle>
NavigationGraph::find_segments(const Vector& pos, float radius)
{
  std::vector<SegmentHandle> ret;
 
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      float distance = Line((*i)->get_node1()->get_pos(),
                            (*i)->get_node2()->get_pos()).distance(pos);
      if (distance < radius)
        {
          ret.push_back(SegmentHandle(*i));
        }
    }

  return ret;
}

NodeHandle
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
  
  return NodeHandle(node);
}

SegmentHandle
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

  return SegmentHandle(segment);
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

void
NavigationGraph::save(std::ostream& out)
{
  int id = 1;
  std::map<Node*, int> ptr2id;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    ptr2id[*i] = id++;

  out << "(navigation\n";

  out << "  (segments\n";
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)  
    out << "    (segment "
        << "(node " << ptr2id[(*i)->get_node1()] << ") "
        << "(node " << ptr2id[(*i)->get_node2()] << ") "
        << "(properties " << (*i)->get_properties() << "))\n";
  out << " )\n";
      
    out << "  (nodes\n"; 
  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    out << "    (node (id " << ptr2id[*i] << ") (pos " << (*i)->get_pos().x << " " << (*i)->get_pos().y << "))\n";
  out << " )\n";

  out << ")\n";
}

bool
NavigationGraph::valid(Segment* segment)
{
  // FIXME: Slow
  return std::find(segments.begin(), segments.end(), segment) != segments.end();
}

bool
NavigationGraph::valid(Node* node)
{
  // FIXME: Slow
  return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
}

/* EOF */
