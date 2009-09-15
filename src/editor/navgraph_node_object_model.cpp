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
#include "editor/constants.hpp"

NavGraphNodeObjectModel::NavGraphNodeObjectModel(const FileReader& reader)
  : ObjectModel(reader),
    m_drawable()
{
}

NavGraphNodeObjectModel::NavGraphNodeObjectModel(const Vector2f& pos)
  : ObjectModel("NavGraphNodeObjectModel", pos),
    m_drawable()
{
}

NavGraphNodeObjectModel::~NavGraphNodeObjectModel()
{
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
}

Rectf
NavGraphNodeObjectModel::get_bounding_box() const
{
  return Rectf(get_world_pos() - Vector2f(10.0f, 10.0f),
               Sizef(20.0f, 20.0f));
}

static float float_snap_to_grid(float v, float grid)
{ 
  return (roundf(v / grid) * grid) - v;
}

SnapData
NavGraphNodeObjectModel::snap_to_grid(float grid_size) const
{
  std::cout << "NavGraphNodeObjectModel:grid" << std::endl;

  float snap_x = float_snap_to_grid(get_world_pos().x, grid_size);
  float snap_y = float_snap_to_grid(get_world_pos().y, grid_size);

  SnapData snap;

  if (fabs(snap_x) < g_snap_threshold)
    {
      snap.x_set = true;
      snap.offset.x = snap_x;
    }

  if (fabs(snap_y) < g_snap_threshold)
    {
      snap.y_set = true;
      snap.offset.y = snap_y;
    }

  return snap;
}

SnapData
NavGraphNodeObjectModel::snap_to_object(const Rectf& in) const
{
  const Vector2f& pos = get_world_pos();

  SnapData snap;

  Rectf dist(in.left   - pos.x,
             in.top    - pos.y,
             in.right  - pos.x,
             in.bottom - pos.y);

  if (fabsf(dist.left) <= fabsf(dist.right) && 
      fabsf(dist.left) < g_snap_threshold)
  {
    snap.x_set    = true;
    snap.offset.x = dist.left;    
  }
  else if (fabsf(dist.left) > fabsf(dist.right) && 
           fabsf(dist.right) < g_snap_threshold)
  {
    snap.x_set    = true;
    snap.offset.x = dist.right;
  }

  if (fabsf(dist.top) <= fabsf(dist.bottom) && 
      fabsf(dist.top) < g_snap_threshold)
  {
    snap.y_set    = true;
    snap.offset.y = dist.top;    
  }
  else if (fabsf(dist.top) > fabsf(dist.bottom) && 
           fabsf(dist.bottom) < g_snap_threshold)
  {
    snap.y_set    = true;
    snap.offset.y = dist.bottom;
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
  writer.start_section("navgraph-node");
  ObjectModel::write_member(writer);
  writer.end_section();
}

/* EOF */
