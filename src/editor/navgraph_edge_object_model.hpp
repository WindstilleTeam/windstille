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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_EDGE_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_EDGE_OBJECT_MODEL_HPP

#include <boost/weak_ptr.hpp>

#include "navigation/navigation_graph.hpp"
#include "editor/object_model.hpp"

class NavGraphNodeObjectModel;
class VertexArrayDrawable;

class NavGraphEdgeObjectModel : public ObjectModel
{
private:
  boost::shared_ptr<NavGraphNodeObjectModel> m_lhs;
  boost::shared_ptr<NavGraphNodeObjectModel> m_rhs;
  boost::shared_ptr<VertexArrayDrawable> m_drawable;
  EdgeHandle m_edge;

public:
  NavGraphEdgeObjectModel(boost::shared_ptr<NavGraphNodeObjectModel> lhs,
                          boost::shared_ptr<NavGraphNodeObjectModel> rhs,
                          SectorModel& sector);

  void add_to_scenegraph(SceneGraph& sg);
  void update(float delta);

  Rectf get_bounding_box() const;
  ObjectModelHandle clone() const;
  void write(FileWriter& writer) const;

  EdgeHandle get_edge() const { return m_edge; }

  boost::shared_ptr<NavGraphNodeObjectModel> get_lhs() const { return m_lhs; }
  boost::shared_ptr<NavGraphNodeObjectModel> get_rhs() const { return m_rhs; }

private:
  NavGraphEdgeObjectModel(const NavGraphEdgeObjectModel&);
  NavGraphEdgeObjectModel& operator=(const NavGraphEdgeObjectModel&);
};

#endif

/* EOF */
