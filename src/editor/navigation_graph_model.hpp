/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "editor/selection.hpp"
#include "math/vector2f.hpp"

class NavigationGraph;
class NavGraphEdgeObjectModel;
class NavGraphNodeObjectModel;
class SelectMask;

class NavigationGraphModel
{
public:
  typedef std::vector<boost::shared_ptr<NavGraphNodeObjectModel> > Nodes;
  typedef std::vector<boost::shared_ptr<NavGraphEdgeObjectModel> > Edges;

private:
  SectorModel& m_sector;
  Nodes m_nodes;
  Edges m_edges;

public:
  NavigationGraphModel(SectorModel& sector);
  ~NavigationGraphModel();

  const Nodes& get_nodes() const { return m_nodes; }

  void add_node(boost::shared_ptr<NavGraphNodeObjectModel> node);
  void add_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge);

  void remove_node(boost::shared_ptr<NavGraphNodeObjectModel> node);
  void remove_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge);

  boost::shared_ptr<NavGraphNodeObjectModel> find_closest_node(const Vector2f& pos, float radius) const;
  boost::shared_ptr<NavGraphEdgeObjectModel> find_closest_edge(const Vector2f& pos, float radius) const;

  std::vector<boost::shared_ptr<NavGraphEdgeObjectModel> > find_edges(boost::shared_ptr<NavGraphNodeObjectModel> node) const;

  boost::shared_ptr<NavGraphNodeObjectModel> get_object_at(const Vector2f& pos, const SelectMask& layers) const;
  SelectionHandle   get_selection(const Rectf& rect, const SelectMask& layers) const;

  bool has_edge(boost::shared_ptr<NavGraphNodeObjectModel> lhs, boost::shared_ptr<NavGraphNodeObjectModel> rhs) const;

  void write(FileWriter& writer) const;
  void load(const FileReader& reader, std::map<std::string, ObjectModelHandle>& id_table);

private:
  NavigationGraphModel(const NavigationGraphModel&);
  NavigationGraphModel& operator=(const NavigationGraphModel&);
};

#endif

/* EOF */
