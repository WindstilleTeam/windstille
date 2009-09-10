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

class Vector2f;
class NavigationGraph;
class NavGraphEdgeObjectModel;
class NavGraphNodeObjectModel;

class NavigationGraphModel
{
private:
  typedef std::vector<boost::shared_ptr<NavGraphNodeObjectModel> > Nodes; 
  typedef std::vector<boost::shared_ptr<NavGraphEdgeObjectModel> > Edges;

  Nodes nodes;
  Edges edges;

public:
  NavigationGraphModel();
  ~NavigationGraphModel();
  
  boost::shared_ptr<NavGraphNodeObjectModel> create_node(const Vector2f& pos);
  boost::shared_ptr<NavGraphEdgeObjectModel> create_edge(boost::shared_ptr<NavGraphNodeObjectModel> lhs, 
                                                         boost::shared_ptr<NavGraphNodeObjectModel> rhs);

  void add_node(boost::shared_ptr<NavGraphNodeObjectModel> node);
  void add_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge);
  
  void remove_node(boost::shared_ptr<NavGraphNodeObjectModel> node);
  void remove_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge);

  boost::shared_ptr<NavGraphNodeObjectModel> find_closest_node(const Vector2f& pos, float radius) const;
  boost::shared_ptr<NavGraphEdgeObjectModel> find_closest_edge(const Vector2f& pos, float radius) const;

private:
  NavigationGraphModel(const NavigationGraphModel&);
  NavigationGraphModel& operator=(const NavigationGraphModel&);
};

#endif

/* EOF */
