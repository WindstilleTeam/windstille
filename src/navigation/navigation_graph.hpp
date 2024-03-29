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

#include <glm/glm.hpp>

#include <geom/geom.hpp>
#include <wstdisplay/fwd.hpp>

#include "util/file_writer.hpp"
#include "util/file_reader.hpp"

namespace windstille {

class Edge;
class EdgePosition;
class FileReader;
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

  inline Data const* operator->() const {
    return data;
  }

  inline Data& operator*()  {
    return *data;
  }

  inline Data const& operator*() const {
    return *data;
  }

  explicit operator bool() {
    return data != nullptr;
  }

  bool operator==(PointerHandle<Data> const& rhs) const {
    return data == rhs.data;
  }

  bool operator<(PointerHandle<Data> const& other) const {
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
  NodeHandle add_node(glm::vec2 const& pos);
  EdgeHandle add_edge(NodeHandle node1, NodeHandle node2);

  void remove_node(NodeHandle node);
  void remove_edge(EdgeHandle edge);

  void split_edge(EdgeHandle edge);

  /** Find edges that intersect with the given line */
  std::vector<EdgePosition> find_intersections(geom::fline const& line);

  /** Find nodes that are near within the \a radius */
  std::vector<NodeHandle> find_nodes(glm::vec2 const& pos, float radius);

  std::vector<NodeHandle> find_nodes(geom::frect const& rect);

  /** Find the closest node, limit search to nodes in radius */
  NodeHandle find_closest_node(glm::vec2 const& pos, float radius);

  EdgeHandle find_closest_edge(glm::vec2 const& pos, float radius);

  /** Find edges that are near the given point */
  std::vector<EdgeHandle> find_edges(glm::vec2 const& pos, float radius);

  /** Draw the navigation graph, for debugging only */
  void draw(wstdisplay::GraphicsContext& gc);

  void load(ReaderMapping const& reader);
  void save(std::ostream& out);
  void write(FileWriter& writer);

  bool valid(Edge* edge);
  bool valid(Node* node);

private:
  NavigationGraph (NavigationGraph const&);
  NavigationGraph& operator= (NavigationGraph const&);
};


} // namespace windstille

#endif

/* EOF */
