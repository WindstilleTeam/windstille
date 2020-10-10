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

#ifndef HEADER_WINDSTILLE_NAVIGATION_NAVIGATION_GRAPH_HPP
#define HEADER_WINDSTILLE_NAVIGATION_NAVIGATION_GRAPH_HPP

#include <iosfwd>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "math/rect.hpp"
#include "math/vector2f.hpp"
#include "util/file_writer.hpp"

class Edge;
class EdgePosition;
class FileReader;
class Line;
class Node;

template<typename Data>
class PointerHandle
{
private:
  Data* data;

public:
  PointerHandle()
    : data(nullptr)
  {}

  PointerHandle(Data* data_)
    : data(data_)
  {}

  inline Data* get() {
    return data;
  }

  inline Data* operator->()  {
    return data;
  }

  inline const Data* operator->() const {
    return data;
  }

  inline Data& operator*()  {
    return *data;
  }

  inline const Data& operator*() const {
    return *data;
  }

  operator bool() {
    return data != nullptr;
  }

  bool operator==(const PointerHandle<Data>& rhs) const {
    return data == rhs.data;
  }

  bool operator<(const PointerHandle<Data>& other) const {
    return this->data < other.data;
  }
};


typedef PointerHandle<Node> NodeHandle;
typedef PointerHandle<Edge> EdgeHandle;

class NavigationGraph
{
public:
  typedef std::vector<Node*> Nodes;
  typedef std::vector<Edge*> Edges;

private:
  Nodes nodes;
  Edges edges;

  // insert some spartial thingy here

public:
  NavigationGraph();
  ~NavigationGraph();

  Nodes& get_nodes() { return nodes; }
  Edges& get_edges() { return edges; }

  // FIXME: It might be worth it to return handles that can be
  // validated instead of pure pointers
  NodeHandle add_node(const Vector2f& pos);
  EdgeHandle add_edge(NodeHandle node1, NodeHandle node2);

  void remove_node(NodeHandle node);
  void remove_edge(EdgeHandle edge);

  void split_edge(EdgeHandle edge);

  /** Find edges that intersect with the given line */
  std::vector<EdgePosition> find_intersections(const Line& line);

  /** Find nodes that are near within the \a radius */
  std::vector<NodeHandle> find_nodes(const Vector2f& pos, float radius);

  std::vector<NodeHandle> find_nodes(const Rectf& rect);

  /** Find the closest node, limit search to nodes in radius */
  NodeHandle find_closest_node(const Vector2f& pos, float radius);

  EdgeHandle find_closest_edge(const Vector2f& pos, float radius);

  /** Find edges that are near the given point */
  std::vector<EdgeHandle> find_edges(const Vector2f& pos, float radius);

  /** Draw the navigation graph, for debugging only */
  void draw();

  void load(FileReader& reader);
  void save(std::ostream& out);
  void write(FileWriter& writer);

  bool valid(Edge* edge);
  bool valid(Node* node);

private:
  NavigationGraph (const NavigationGraph&);
  NavigationGraph& operator= (const NavigationGraph&);
};

#endif

/* EOF */
