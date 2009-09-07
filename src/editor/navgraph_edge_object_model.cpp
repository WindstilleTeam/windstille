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

#include "editor/navgraph_edge_object_model.hpp"

#include "editor/navgraph_node_object_model.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "scenegraph/scene_graph.hpp"

NavGraphEdgeObjectModel::NavGraphEdgeObjectModel(boost::weak_ptr<NavGraphNodeObjectModel> lhs,
                                                 boost::weak_ptr<NavGraphNodeObjectModel> rhs)
  : ObjectModel("NavGraphEdgeObjectModel", Vector2f()),
    m_lhs(lhs),
    m_rhs(rhs),
    m_drawable()
{
}

void
NavGraphEdgeObjectModel::update(float delta)
{
  if (m_drawable)
  {
    boost::shared_ptr<NavGraphNodeObjectModel> lhs = m_lhs.lock();
    boost::shared_ptr<NavGraphNodeObjectModel> rhs = m_rhs.lock();

    if (lhs && rhs)
    {
      m_drawable->clear();

      m_drawable->color(Color(0.0f, 1.0f, 1.0f));
      m_drawable->vertex(lhs->get_world_pos());

      m_drawable->color(Color(0.0f, 1.0f, 1.0f));
      m_drawable->vertex(rhs->get_world_pos());
    }
  }
}

void
NavGraphEdgeObjectModel::add_to_scenegraph(SceneGraph& sg)
{
  m_drawable.reset(new VertexArrayDrawable(Vector2f(), 0.0f, Matrix::identity()));
  update(0.0f);
  sg.add_drawable(m_drawable);
}

/* EOF */
