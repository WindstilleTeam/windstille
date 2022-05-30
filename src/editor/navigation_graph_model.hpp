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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVIGATION_GRAPH_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVIGATION_GRAPH_MODEL_HPP

#include <vector>
#include <memory>
#include <memory>

#include "editor/selection.hpp"
#include <glm/glm.hpp>

class NavigationGraph;
class NavGraphEdgeObjectModel;
class NavGraphNodeObjectModel;
class SelectMask;

class NavigationGraphModel
{
public:
  typedef std::vector<std::shared_ptr<NavGraphNodeObjectModel> > Nodes;
  typedef std::vector<std::shared_ptr<NavGraphEdgeObjectModel> > Edges;

private:
  SectorModel& m_sector;
  Nodes m_nodes;
  Edges m_edges;

public:
  NavigationGraphModel(SectorModel& sector);
  ~NavigationGraphModel();

  Nodes const& get_nodes() const { return m_nodes; }

  void add_node(std::shared_ptr<NavGraphNodeObjectModel> node);
  void add_edge(std::shared_ptr<NavGraphEdgeObjectModel> edge);

  void remove_node(std::shared_ptr<NavGraphNodeObjectModel> node);
  void remove_edge(std::shared_ptr<NavGraphEdgeObjectModel> edge);

  std::shared_ptr<NavGraphNodeObjectModel> find_closest_node(glm::vec2 const& pos, float radius) const;
  std::shared_ptr<NavGraphEdgeObjectModel> find_closest_edge(glm::vec2 const& pos, float radius) const;

  std::vector<std::shared_ptr<NavGraphEdgeObjectModel> > find_edges(std::shared_ptr<NavGraphNodeObjectModel> node) const;

  std::shared_ptr<NavGraphNodeObjectModel> get_object_at(glm::vec2 const& pos, SelectMask const& layers) const;
  SelectionHandle   get_selection(geom::frect const& rect, SelectMask const& layers) const;

  bool has_edge(std::shared_ptr<NavGraphNodeObjectModel> lhs, std::shared_ptr<NavGraphNodeObjectModel> rhs) const;

  void write(FileWriter& writer) const;
  void load(ReaderMapping const& reader, std::map<std::string, ObjectModelHandle>& id_table);

private:
  NavigationGraphModel(NavigationGraphModel const&);
  NavigationGraphModel& operator=(NavigationGraphModel const&);
};

#endif

/* EOF */
