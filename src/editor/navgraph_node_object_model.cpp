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

#include "editor/navgraph_node_object_model.hpp"

#include "editor/sector_model.hpp"
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>
#include <wstdisplay/scenegraph/drawable_group.hpp>
#include "navigation/node.hpp"
#include "editor/constants.hpp"

namespace windstille {

NavGraphNodeObjectModel::NavGraphNodeObjectModel(ReaderMapping const& reader) :
  ObjectModel(reader),
  m_drawable()
{
}

NavGraphNodeObjectModel::NavGraphNodeObjectModel(glm::vec2 const& pos) :
  ObjectModel("NavGraphNodeObjectModel", pos),
  m_drawable()
{
}

NavGraphNodeObjectModel::~NavGraphNodeObjectModel()
{
}

void
NavGraphNodeObjectModel::add_to_scenegraph(wstdisplay::DrawableGroup& sg)
{
  if (!m_drawable)
    m_drawable.reset(new wstdisplay::VertexArrayDrawable);

  sync_drawable();
  sg.add_drawable(m_drawable);
}

void
NavGraphNodeObjectModel::sync_drawable()
{
  if (m_drawable)
  {
    m_drawable->clear();

    m_drawable->set_mode(GL_TRIANGLE_FAN);

    m_drawable->color(surf::Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + glm::vec2(-10.0f, -10.0f));

    m_drawable->color(surf::Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + glm::vec2(10.0f, -10.0f));

    m_drawable->color(surf::Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + glm::vec2(10.0f, 10.0f));

    m_drawable->color(surf::Color(1.0f, 0.0f, 0.0f));
    m_drawable->vertex(get_world_pos() + glm::vec2(-10.0f, 10.0f));
  }
}

void
NavGraphNodeObjectModel::set_rel_pos(glm::vec2 const& rel_pos_)
{
  ObjectModel::set_rel_pos(rel_pos_);
}

geom::frect
NavGraphNodeObjectModel::get_bounding_box() const
{
  return geom::frect(get_world_pos() - glm::vec2(10.0f, 10.0f),
               geom::fsize(20.0f, 20.0f));
}

static float float_snap_to_grid(float v, float grid)
{
  return (roundf(v / grid) * grid) - v;
}

SnapData
NavGraphNodeObjectModel::snap_to_grid(float grid_size) const
{
  float snap_x = float_snap_to_grid(get_world_pos().x, grid_size);
  float snap_y = float_snap_to_grid(get_world_pos().y, grid_size);

  SnapData snap;

  if (fabsf(snap_x) < g_snap_threshold)
  {
    snap.set_x(snap_x);
  }

  if (fabsf(snap_y) < g_snap_threshold)
  {
    snap.set_y(snap_y);
  }

  return snap;
}

SnapData
NavGraphNodeObjectModel::snap_to_object(geom::frect const& in) const
{
  glm::vec2 const& pos = get_world_pos();

  SnapData snap;

  geom::frect dist(in.left()   - pos.x,
             in.top()    - pos.y,
             in.right()  - pos.x,
             in.bottom() - pos.y);

  if (pos.y > in.top() - g_snap_threshold &&
      pos.y < in.bottom() + g_snap_threshold)
  {
    if (fabsf(dist.left()) <= fabsf(dist.right()) &&
        fabsf(dist.left()) < g_snap_threshold)
    {
      snap.set_x(dist.left());
    }
    else if (fabsf(dist.left()) > fabsf(dist.right()) &&
             fabsf(dist.right()) < g_snap_threshold)
    {
      snap.set_x(dist.right());
    }
  }

  if (pos.x > in.left()  - g_snap_threshold &&
      pos.x < in.right() + g_snap_threshold)
  {
    if (fabsf(dist.top()) <= fabsf(dist.bottom()) &&
        fabsf(dist.top()) < g_snap_threshold)
    {
      snap.set_y(dist.top());
    }
    else if (fabsf(dist.top()) > fabsf(dist.bottom()) &&
             fabsf(dist.bottom()) < g_snap_threshold)
    {
      snap.set_y(dist.bottom());
    }
  }

  return snap;
}

ObjectModelHandle
NavGraphNodeObjectModel::clone() const
{
  return ObjectModelHandle(new NavGraphNodeObjectModel(get_world_pos()));
}

void
NavGraphNodeObjectModel::write(FileWriter& writer) const
{
  writer.begin_object("node");
  ObjectModel::write_member(writer);
  writer.end_object();
}

} // namespace windstille

/* EOF */
