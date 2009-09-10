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

#include "editor/navigation_graph_model.hpp"

#include "editor/navgraph_edge_object_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "math/line.hpp"
#include "navigation/navigation_graph.hpp"

NavigationGraphModel::NavigationGraphModel()
  : nodes(),
    edges()   
{
}

NavigationGraphModel::~NavigationGraphModel()
{
}
  
boost::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::create_node(const Vector2f& pos)
{
  boost::shared_ptr<NavGraphNodeObjectModel> node(new NavGraphNodeObjectModel(pos));
  nodes.push_back(node);
  return node;
}

boost::shared_ptr<NavGraphEdgeObjectModel>
NavigationGraphModel::create_edge(boost::shared_ptr<NavGraphNodeObjectModel> lhs, 
                                  boost::shared_ptr<NavGraphNodeObjectModel> rhs)
{
  boost::shared_ptr<NavGraphEdgeObjectModel> edge(new NavGraphEdgeObjectModel(lhs, rhs));
  edges.push_back(edge);
  return edge;
}

  
void
NavigationGraphModel::remove_node(boost::shared_ptr<NavGraphNodeObjectModel> node)
{
}

void
NavigationGraphModel::remove_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge)
{
}

boost::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::find_closest_node(const Vector2f& pos, float radius) const
{
  boost::shared_ptr<NavGraphNodeObjectModel> node;
  float min_distance = radius;

  for(Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
  {
    float current_distance = (pos - (*i)->get_world_pos()).length();
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      node = *i;
    }
  }

  return node;
}

boost::shared_ptr<NavGraphEdgeObjectModel>
NavigationGraphModel::find_closest_edge(const Vector2f& pos, float radius) const
{
  boost::shared_ptr<NavGraphEdgeObjectModel> edge;
  float min_distance = radius;

  for(Edges::const_iterator i = edges.begin(); i != edges.end(); ++i)
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

/* EOF */
