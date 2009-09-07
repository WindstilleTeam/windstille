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

#include "editor/object_model.hpp"

class NavGraphNodeObjectModel;
class VertexArrayDrawable;

class NavGraphEdgeObjectModel : public ObjectModel
{
private:
  boost::weak_ptr<NavGraphNodeObjectModel> m_lhs;
  boost::weak_ptr<NavGraphNodeObjectModel> m_rhs;
  boost::shared_ptr<VertexArrayDrawable> m_drawable;
  
public:
  NavGraphEdgeObjectModel(boost::weak_ptr<NavGraphNodeObjectModel> lhs,
                          boost::weak_ptr<NavGraphNodeObjectModel> rhs);

  void add_to_scenegraph(SceneGraph& sg);
  void update(float delta);

private:
  NavGraphEdgeObjectModel(const NavGraphEdgeObjectModel&);
  NavGraphEdgeObjectModel& operator=(const NavGraphEdgeObjectModel&);
};

#endif

/* EOF */
