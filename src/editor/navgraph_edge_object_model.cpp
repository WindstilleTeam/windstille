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

#include "editor/navgraph_edge_object_model.hpp"

#include "editor/navgraph_node_object_model.hpp"
#include "editor/sector_model.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/drawable_group.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

NavGraphEdgeObjectModel::NavGraphEdgeObjectModel(std::shared_ptr<NavGraphNodeObjectModel> lhs,
                                                 std::shared_ptr<NavGraphNodeObjectModel> rhs) :
  ObjectModel("NavGraphEdgeObjectModel", glm::vec2()),
  m_lhs(lhs),
  m_rhs(rhs),
  m_drawable()
{
  if (m_lhs == m_rhs)
  {
    throw std::runtime_error("NavGraphEdgeObjectModel: lhs and rhs must not be the same");
  }

#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
  // We enforce order so that we can easier compare
  // NavGraphEdgeObjectModel's with one another
  if (!(m_lhs < m_rhs)) // NOLINT
    std::swap(m_lhs, m_rhs);
}

NavGraphEdgeObjectModel::~NavGraphEdgeObjectModel()
{
}

void
NavGraphEdgeObjectModel::update(float delta)
{
  if (m_drawable)
  {
    m_drawable->clear();
    m_drawable->set_mode(GL_LINES);

    m_drawable->color(Color(0.0f, 1.0f, 1.0f));
    m_drawable->vertex(m_lhs->get_world_pos());

    m_drawable->color(Color(0.0f, 1.0f, 1.0f));
    m_drawable->vertex(m_rhs->get_world_pos());
  }
}

void
NavGraphEdgeObjectModel::add_to_scenegraph(DrawableGroup& sg)
{
  m_drawable.reset(new VertexArrayDrawable);
  update(0.0f);
  sg.add_drawable(m_drawable);
}

geom::frect
NavGraphEdgeObjectModel::get_bounding_box() const
{
  geom::frect rect(m_lhs->get_world_pos(),
             m_rhs->get_world_pos());
  rect = geom::normalize(rect);
  return rect;
}

ObjectModelHandle
NavGraphEdgeObjectModel::clone() const
{
  // FIXME: This is incorrect for copy paste, as there we need to
  // connect to the cloned nodes, not the old ones
  return ObjectModelHandle(new NavGraphEdgeObjectModel(m_lhs, m_rhs));
}

void
NavGraphEdgeObjectModel::write(FileWriter& writer) const
{
  writer.begin_object("navgraph-edge-ref");
  writer.write("edge", get_id());
  writer.end_object();
}

void
NavGraphEdgeObjectModel::write_real(FileWriter& writer) const
{
  writer.begin_object("edge");
  ObjectModel::write_member(writer);
  writer.write("lhs-node", m_lhs->get_id());
  writer.write("rhs-node", m_rhs->get_id());
  writer.end_object();
}

/* EOF */
