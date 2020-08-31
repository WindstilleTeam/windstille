/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/navigation_graph_model.hpp"

#include "editor/navgraph_edge_object_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "math/line.hpp"
#include "navigation/navigation_graph.hpp"
#include "util/file_reader.hpp"

NavigationGraphModel::NavigationGraphModel(SectorModel& sector) :
  m_sector(sector),
  m_nodes(),
  m_edges()
{
}

NavigationGraphModel::~NavigationGraphModel()
{
}

void
NavigationGraphModel::add_node(std::shared_ptr<NavGraphNodeObjectModel> node)
{
  m_nodes.push_back(node);
}

void
NavigationGraphModel::add_edge(std::shared_ptr<NavGraphEdgeObjectModel> edge)
{
  m_edges.push_back(edge);
}

struct EdgeHasNode
{
  std::shared_ptr<NavGraphNodeObjectModel> m_node;

  EdgeHasNode(std::shared_ptr<NavGraphNodeObjectModel> node)
    : m_node(node)
  {}

  bool operator()(std::shared_ptr<NavGraphEdgeObjectModel> edge)
  {
    return
      edge->get_lhs() == m_node ||
      edge->get_rhs() == m_node;
  }
};

void
NavigationGraphModel::remove_node(std::shared_ptr<NavGraphNodeObjectModel> node)
{
  std::cout << "remove_node: " << m_nodes.size() << std::endl;
  m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), node), m_nodes.end());
  m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), EdgeHasNode(node)), m_edges.end());
  std::cout << "remove_node: after: " << m_nodes.size() << std::endl;
}

void
NavigationGraphModel::remove_edge(std::shared_ptr<NavGraphEdgeObjectModel> edge)
{
  m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());
}

std::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::find_closest_node(const Vector2f& pos, float radius) const
{
  std::shared_ptr<NavGraphNodeObjectModel> node;
  float min_distance = radius;

  for(Nodes::const_iterator i = m_nodes.begin(); i != m_nodes.end(); ++i)
  {
    float current_distance = glm::length(pos - (*i)->get_world_pos());
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      node = *i;
    }
  }

  return node;
}

std::shared_ptr<NavGraphEdgeObjectModel>
NavigationGraphModel::find_closest_edge(const Vector2f& pos, float radius) const
{
  std::shared_ptr<NavGraphEdgeObjectModel> edge;
  float min_distance = radius;

  for(Edges::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
  {
    float current_distance = Line((*i)->get_lhs()->get_world_pos(),
                                  (*i)->get_rhs()->get_world_pos()).distance(pos);
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      edge = *i;
    }
  }

  return edge;
}

std::vector<std::shared_ptr<NavGraphEdgeObjectModel> >
NavigationGraphModel::find_edges(std::shared_ptr<NavGraphNodeObjectModel> node) const
{
  std::vector<std::shared_ptr<NavGraphEdgeObjectModel> > edges;

  for(Edges::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
  {
    if ((*i)->get_lhs() == node || (*i)->get_rhs() == node)
    {
      edges.push_back(*i);
    }
  }

  return edges;
}

std::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::get_object_at(const Vector2f& pos, const SelectMask& select_mask) const
{
  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        (*i)->is_at(pos))
    {
      return *i;
    }
  }

  return std::shared_ptr<NavGraphNodeObjectModel>();
}

SelectionHandle
NavigationGraphModel::get_selection(const Rectf& rect, const SelectMask& select_mask) const
{
  SelectionHandle selection = Selection::create();

  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        rect.contains((*i)->get_bounding_box()))
    {
      selection->add(*i);
    }
  }

  return selection;
}

bool
NavigationGraphModel::has_edge(std::shared_ptr<NavGraphNodeObjectModel> lhs, std::shared_ptr<NavGraphNodeObjectModel> rhs) const
{
  // We enforce order so that we can easier compare
  // NavGraphEdgeObjectModel's with one another
  if (!(lhs < rhs))
    std::swap(lhs, rhs);

  for(Edges::const_reverse_iterator i = m_edges.rbegin(); i != m_edges.rend(); ++i)
  {
    if ((*i)->get_lhs() == lhs &&
        (*i)->get_rhs() == rhs)
      return true;
  }

  return false;
}

void
NavigationGraphModel::write(FileWriter& writer) const
{
  writer.start_section("nodes");
  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    (*i)->write(writer);
  }
  writer.end_section();

  writer.start_section("edges");
  for(Edges::const_reverse_iterator i = m_edges.rbegin(); i != m_edges.rend(); ++i)
  {
    (*i)->write_real(writer);
  }
  writer.end_section();
}

void
NavigationGraphModel::load(const FileReader& reader, std::map<std::string, ObjectModelHandle>& all_id_table)
{
  std::cout << "NavigationGraphModel::load" << std::endl;
  std::map<std::string, std::shared_ptr<NavGraphNodeObjectModel> > id_table;

  FileReader nodes_section;
  if (reader.get("nodes", nodes_section))
  {
    const std::vector<FileReader>& sections = nodes_section.get_sections();
    for(std::vector<FileReader>::const_iterator i = sections.begin(); i != sections.end(); ++i)
    {
      if (i->get_name() == "navgraph-node")
      {
        std::shared_ptr<NavGraphNodeObjectModel> node(new NavGraphNodeObjectModel(*i));

        std::string id_str;
        if (i->get("id", id_str))
        {
          id_table[id_str] = node;
        }

        m_nodes.push_back(node);
      }
      else
      {
        std::cout << "NavigationGraphModel:load: unknown tag " << i->get_name() << std::endl;
      }
    }
  }
  else
  {
    std::cout << "NavigationGraphModel: nodes missing" << std::endl;
  }

  FileReader edges_section;
  if (reader.get("edges", edges_section))
  {
    const std::vector<FileReader>& sections = edges_section.get_sections();
    for(std::vector<FileReader>::const_iterator i = sections.begin(); i != sections.end(); ++i)
    {
      if (i->get_name() == "navgraph-edge")
      {
        std::string lhs;
        std::string rhs;

        if (i->get("lhs-node", lhs) && i->get("rhs-node", rhs))
        {
          std::shared_ptr<NavGraphNodeObjectModel> lhs_node = id_table[lhs];
          std::shared_ptr<NavGraphNodeObjectModel> rhs_node = id_table[rhs];

          if (lhs_node && rhs_node)
          {
            std::shared_ptr<NavGraphEdgeObjectModel> edge(new NavGraphEdgeObjectModel(lhs_node, rhs_node));

            std::string id_str;
            if (i->get("id", id_str))
            {
              all_id_table[id_str] = edge;
            }

            m_edges.push_back(edge);
          }
          else
          {
            std::cout << "Error: NavigationGraphModel::load: couldn't resolve edge: " << lhs_node << " " << rhs_node << std::endl;
          }
        }
        else
        {
          std::cout << "Error: NavigationGraphModel::load: invalid edge" << std::endl;
        }
      }
      else
      {
        std::cout << "NavigationGraphModel:load: unknown tag " << i->get_name() << std::endl;
      }
    }
  }
  else
  {
    std::cout << "NavigationGraphModel: edges missing" << std::endl;
  }
}

/* EOF */
