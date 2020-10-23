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

#include "navigation/navigation_graph.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>

#include <geom/geom.hpp>

#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/color.hpp>
#include "navigation/edge.hpp"
#include "navigation/node.hpp"
#include "util/file_writer.hpp"
#include "util/file_reader.hpp"

NavigationGraph::NavigationGraph() :
  nodes(),
  edges()
{
}

NavigationGraph::~NavigationGraph()
{
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
    delete *i;
  edges.clear();

  for (Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    delete *i;
  nodes.clear();
}

NodeHandle
NavigationGraph::add_node(const glm::vec2& pos)
{
  Node* node = new Node(pos);
  nodes.push_back(node);
  return NodeHandle(node);
}

void
NavigationGraph::remove_edge(EdgeHandle edge)
{
  // FIXME: Slow
  Edges::iterator i = std::find(edges.begin(), edges.end(), edge.get());
  if (i != edges.end())
  {
    edges.erase(i);
    delete edge.get();
  }

  // FIXME: Throw exception here
}

void
NavigationGraph::remove_node(NodeHandle node)
{
  // FIXME: Slow

  // Remove all edges that would get invalid by removing the node
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    if ((*i)->get_node1() == node.get() ||
        (*i)->get_node2() == node.get())
    {
      delete *i;
      *i = nullptr;
    }
  }

  Edges::iterator new_end = std::remove(edges.begin(), edges.end(), static_cast<Edge*>(nullptr));
  if (new_end != edges.end())
  {
    edges.erase(new_end, edges.end());
  }

  // Remove the node itself
  Nodes::iterator j = std::find(nodes.begin(), nodes.end(), node.get());
  if (j != nodes.end())
  {
    nodes.erase(j);
    delete node.get();
  }
}

EdgeHandle
NavigationGraph::add_edge(NodeHandle node1, NodeHandle node2)
{
  if (node1.get() != node2.get()) // node links to themself are forbidden
  { // FIXME: Find a way to figure out if the given edge already exists
    Edge* edge = new Edge(node1.get(), node2.get());
    edges.push_back(edge);
    return EdgeHandle(edge);
  }
  else
  {
    return EdgeHandle();
  }
}

void
NavigationGraph::split_edge(EdgeHandle edge)
{
  NodeHandle node1 = NodeHandle(edge->get_node1());
  NodeHandle node3 = NodeHandle(edge->get_node2());
  NodeHandle node2 = add_node(0.5f * (node1->get_pos() + node3->get_pos()));

  remove_edge(edge);
  add_edge(node1, node2);
  add_edge(node2, node3);
}

std::vector<EdgePosition>
NavigationGraph::find_intersections(const geom::line& line)
{
  // FIXME: we might want to only return the first intersection, not
  // all of them or alternativly return ua
  std::vector<EdgePosition> ret;

  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    geom::line seg_line((*i)->get_node1()->get_pos(),
                  (*i)->get_node2()->get_pos());

    float ua, ub;
    if (line.intersect(seg_line, ua, ub))
    {
      ret.push_back(EdgePosition(*i, ub));
    }
  }

  return ret;
}

std::vector<NodeHandle>
NavigationGraph::find_nodes(const glm::vec2& pos, float radius)
{
  // FIXME: Optimize this with spatial tree thingy
  std::vector<NodeHandle> ret;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    float distance = glm::length(pos - (*i)->get_pos());
    if (distance < radius)
    {
      ret.push_back(NodeHandle(*i));
    }
  }

  return ret;
}

std::vector<NodeHandle>
NavigationGraph::find_nodes(const geom::frect& rect)
{
  std::vector<NodeHandle> ret;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    if (geom::contains(rect, geom::fpoint((*i)->get_pos())))
    {
      ret.push_back(NodeHandle(*i));
    }
  }

  return ret;
}

std::vector<EdgeHandle>
NavigationGraph::find_edges(const glm::vec2& pos, float radius)
{
  std::vector<EdgeHandle> ret;

  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    float distance = geom::line((*i)->get_node1()->get_pos(),
                          (*i)->get_node2()->get_pos()).distance(pos);
    if (distance < radius)
    {
      ret.push_back(EdgeHandle(*i));
    }
  }

  return ret;
}

NodeHandle
NavigationGraph::find_closest_node(const glm::vec2& pos, float radius)
{
  // FIXME: Optimize this with spatial tree thingy
  Node* node = nullptr;
  float min_distance = radius;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    float current_distance = glm::length(pos - (*i)->get_pos());
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      node = *i;
    }
  }

  return NodeHandle(node);
}

EdgeHandle
NavigationGraph::find_closest_edge(const glm::vec2& pos, float radius)
{
  Edge* edge   = nullptr;
  float min_distance = radius;

  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    float current_distance = geom::line((*i)->get_node1()->get_pos(),
                                  (*i)->get_node2()->get_pos()).distance(pos);
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      edge = *i;
    }
  }

  return EdgeHandle(edge);
}

void
NavigationGraph::draw(GraphicsContext& gc)
{
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    gc.draw_line_with_normal(geom::line((*i)->get_node1()->get_pos(),
                                        (*i)->get_node2()->get_pos()),
                                   RGBAf(1.0f, 0.0f, 0.0f));
  }

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    gc.fill_rect(geom::frect((*i)->get_pos() - glm::vec2(4,4), geom::fsize(9, 9)),
                       RGBAf(1.0f, 1.0f, 0.0f));
  }
}

void
NavigationGraph::load(ReaderMapping const& reader)
{
  nodes.clear();
  edges.clear();

  int id_count = 1;
  std::map<int, Node*> id2ptr;

  ReaderCollection nodes_collection;
  if (reader.read("nodes", nodes_collection))
  {
    for(auto const& node_obj : nodes_collection.get_objects())
    {
      if (node_obj.get_name() == "node") {
        ReaderMapping const& node_reader = node_obj.get_mapping();

        glm::vec2 pos;
        if (node_reader.read("pos", pos)) {
          Node* node = new Node(pos);
          id2ptr[id_count++] = node;
          nodes.push_back(node);
        }
      }
      else
      {
        std::cout << "NavigationGraph:load: nodes: Unknown tag: " << node_obj.get_name() << std::endl;
      }
    }
  }

  ReaderCollection edges_collection;
  if (reader.read("edges", edges_collection))
  {
    for(auto const& edge_obj : edges_collection.get_objects())
    {
      if (edge_obj.get_name() == "edge") {
        ReaderMapping const& edge_reader = edge_obj.get_mapping();

        int node_left;
        int node_right;
        int properties;

        if (edge_reader.read("node1", node_left) &&
            edge_reader.read("node2", node_right) &&
            edge_reader.read("properties", properties)) // FIXME: we might want to read a unsigned int instead
        {
          std::map<int, Node*>::iterator node_left_ptr  = id2ptr.find(node_left);
          std::map<int, Node*>::iterator node_right_ptr = id2ptr.find(node_right);

          if (node_left_ptr != id2ptr.end() && node_right_ptr != id2ptr.end())
          {
            Edge* edge = new Edge(node_left_ptr->second, node_right_ptr->second, properties);
            edges.push_back(edge);
          }
          else
          {
            std::cout << "NavigationGraph: edge: Couldn't lookup ids: "
                      << node_left << " " << node_right << std::endl;
          }
        }
        else
        {
          std::cout << "NavigationGraph:load: edges: edge: parse error" << std::endl;
        }
      }
      else
      {
        std::cout << "NavigationGraph:load: edges: Unknown tag: " << edge_obj.get_name() << std::endl;
      }
    }
  }
}

void
NavigationGraph::save(std::ostream& out)
{ // FIXME: Obsolete
  int id = 1;
  std::map<Node*, int> ptr2id;

  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    ptr2id[*i] = id++;

  std::ios_base::fmtflags old_flags = out.flags(); // save flags

  out << "(navigation\n";
  out << "  (nodes\n";
  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
    out << "    (node (id " << std::setw(3) << ptr2id[*i] << ") (pos "
        << std::setw(3) << (*i)->get_pos().x << " " << (*i)->get_pos().y << "))\n";
  out << " )\n";

  out << "  (edges\n";
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
    out << "    (edge "
        << "(node1 " << std::setw(3) << ptr2id[(*i)->get_node1()] << ") "
        << "(node2 " << std::setw(3) << ptr2id[(*i)->get_node2()] << ") "
        << "(properties " << (*i)->get_properties() << "))\n";
  out << " )\n";

  out << ")\n";

  out.flags(old_flags); // restore flags
}

void
NavigationGraph::write(FileWriter& writer)
{
  std::map<Node*, int> ptr2id;

  {
    for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
      ptr2id[*i] = static_cast<int>(i - nodes.begin() + 1);
  }

  writer.begin_collection("nodes");
  for(Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    writer.begin_object("node");
    writer.write("id", ptr2id[*i]);
    writer.write("pos", (*i)->get_pos());
    writer.end_object();
  }
  writer.end_collection();

  writer.begin_collection("edges");
  for(Edges::iterator i = edges.begin(); i != edges.end(); ++i)
  {
    writer.begin_object("edge");
    writer.write("node1", ptr2id[(*i)->get_node1()]);
    writer.write("node2", ptr2id[(*i)->get_node2()]);
    writer.write("properties", static_cast<int>((*i)->get_properties()));
    writer.end_object();
  }
  writer.end_collection();
}

bool
NavigationGraph::valid(Edge* edge)
{
  // FIXME: Slow
  return std::find(edges.begin(), edges.end(), edge) != edges.end();
}

bool
NavigationGraph::valid(Node* node)
{
  // FIXME: Slow
  return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
}

/* EOF */
