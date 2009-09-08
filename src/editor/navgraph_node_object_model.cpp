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

#include "editor/navgraph_node_object_model.hpp"

#include "editor/sector_model.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "scenegraph/scene_graph.hpp"
#include "navigation/node.hpp"

NavGraphNodeObjectModel::NavGraphNodeObjectModel(const Vector2f& pos, SectorModel& sector)
  : ObjectModel("NavGraphNodeObjectModel", pos),
    m_drawable(),
    m_node()
{
  m_node = sector.get_nav_graph()->add_node(pos);
}

void
NavGraphNodeObjectModel::add_to_scenegraph(SceneGraph& sg)
{
  if (!m_drawable)
    m_drawable.reset(new VertexArrayDrawable(Vector2f(), 0.0f, Matrix::identity()));

  sync_drawable();
  sg.add_drawable(m_drawable);
}

void
NavGraphNodeObjectModel::sync_drawable()
{
  if (m_drawable)
  {
    m_drawable->clear();

    m_drawable->color(Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + Vector2f(-10.0f, -10.0f));

    m_drawable->color(Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + Vector2f(10.0f, -10.0f));

    m_drawable->color(Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + Vector2f(10.0f, 10.0f));

    m_drawable->color(Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + Vector2f(-10.0f, 10.0f));
  }
}

void
NavGraphNodeObjectModel::set_rel_pos(const Vector2f& rel_pos_)
{
  ObjectModel::set_rel_pos(rel_pos_);
  m_node->set_pos(get_world_pos());
}

Rectf
NavGraphNodeObjectModel::get_bounding_box() const
{
  return Rectf(get_world_pos() - Vector2f(10.0f, 10.0f),
               Sizef(20.0f, 20.0f));
}

ObjectModelHandle 
NavGraphNodeObjectModel::clone() const
{
  return ObjectModelHandle();
}

void
NavGraphNodeObjectModel::write(FileWriter& writer) const
{
  writer.start_section("navgraph-node");
  ObjectModel::write_member(writer);
  //writer.write("vflip",   vflip);
  writer.end_section();
}

/* EOF */
