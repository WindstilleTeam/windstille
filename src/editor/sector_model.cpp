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

#include "editor/sector_model.hpp"

#include <algorithm>

#include <logmich/log.hpp>

#include "display/scene_context.hpp"
#include "editor/navgraph_edge_object_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "editor/navigation_graph_model.hpp"
#include "editor/sector_model_builder.hpp"
#include "editor/timeline.hpp"
#include "util/file_writer.hpp"

namespace windstille {

SectorModel::SectorModel() :
  nav_graph(new NavigationGraphModel(*this)),
  m_layers(),
  m_timeline(new Timeline()),
  ambient_color(),
  m_signal_layers_changed()
{
  add_layer("Scene");
}

SectorModel::SectorModel(std::string const& filename) :
  nav_graph(new NavigationGraphModel(*this)),
  m_layers(),
  m_timeline(new Timeline()),
  ambient_color(),
  m_signal_layers_changed()
{
  SectorModelBuilder(filename, *this);
}

SectorModel::~SectorModel() = default;

void SectorModel::notify_layers_changed() { m_signal_layers_changed(); }

LayerHandle
SectorModel::add_layer(std::string const& name, std::size_t index)
{
  LayerHandle layer(new Layer(*this));
  layer->set_name(name);
  layer->set_visible(true);
  layer->set_locked(false);
  return add_layer(layer, index);
}

LayerHandle
SectorModel::add_layer(LayerHandle layer, std::size_t index)
{
  if (!layer) return LayerHandle();
  if (index == npos || index >= m_layers.size())
    m_layers.push_back(layer);
  else
    m_layers.insert(m_layers.begin() + static_cast<std::ptrdiff_t>(index), layer);
  notify_layers_changed();
  return layer;
}

void
SectorModel::delete_layer(std::size_t index)
{
  if (index >= m_layers.size()) {
    log_warn("SectorModel::delete_layer: index {} out of range", index);
    return;
  }
  m_layers.erase(m_layers.begin() + static_cast<std::ptrdiff_t>(index));
  notify_layers_changed();
}

void
SectorModel::delete_layer(LayerHandle layer)
{
  auto it = std::find(m_layers.begin(), m_layers.end(), layer);
  if (it == m_layers.end()) {
    log_warn("SectorModel::delete_layer: layer not found");
    return;
  }
  m_layers.erase(it);
  notify_layers_changed();
}

void SectorModel::reverse_layers()
{
  std::reverse(m_layers.begin(), m_layers.end());
  notify_layers_changed();
}

void
SectorModel::move_layer(std::size_t from_index, std::size_t to_index)
{
  if (from_index >= m_layers.size()) return;
  LayerHandle layer = m_layers[from_index];
  m_layers.erase(m_layers.begin() + static_cast<std::ptrdiff_t>(from_index));
  if (to_index > from_index) --to_index;
  if (to_index >= m_layers.size()) m_layers.push_back(layer);
  else m_layers.insert(m_layers.begin() + static_cast<std::ptrdiff_t>(to_index), layer);
  notify_layers_changed();
}

void
SectorModel::add(ObjectModelHandle const& object, LayerHandle layer)
{
  if (!layer) { log_warn("SectorModel::add: null layer"); return; }
  layer->add(object);
  notify_layers_changed();
}

void
SectorModel::remove(ObjectModelHandle const& object)
{
  for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
    (*it)->remove(object);
  notify_layers_changed();
}

LayerHandle
SectorModel::get_layer(std::size_t index) const
{
  if (index >= m_layers.size()) return LayerHandle();
  return m_layers[index];
}

std::size_t
SectorModel::get_layer_index(LayerHandle layer) const
{
  auto it = std::find(m_layers.begin(), m_layers.end(), layer);
  if (it == m_layers.end()) return npos;
  return static_cast<std::size_t>(std::distance(m_layers.begin(), it));
}

LayerHandle
SectorModel::get_layer(ObjectModelHandle const& object) const
{
  for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
    if ((*it)->has_object(object)) return *it;
  return LayerHandle();
}

void
SectorModel::draw(SceneContext& sc, SelectMask const& layermask)
{
  for (auto const& layer : m_layers)
    if (layer->is_visible()) layer->draw(sc, layermask);
}

void
SectorModel::update(float delta)
{
  for (auto const& layer : m_layers)
    if (layer->is_visible()) layer->update(delta);
}

ObjectModelHandle
SectorModel::get_object_at(glm::vec2 const& pos, SelectMask const& layermask) const
{
  if (ObjectModelHandle obj = nav_graph->get_object_at(pos, layermask))
    return obj;
  for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
    if ((*it)->is_visible() && !(*it)->is_locked())
      if (ObjectModelHandle object = (*it)->get_object_at(pos, layermask))
        return object;
  return ObjectModelHandle();
}

SelectionHandle
SectorModel::get_selection(geom::frect const& rect, SelectMask const& layermask) const
{
  SelectionHandle selection = Selection::create();
  {
    SelectionHandle new_sel = nav_graph->get_selection(rect, layermask);
    selection->add(new_sel->begin(), new_sel->end());
  }
  for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
    if ((*it)->is_visible() && !(*it)->is_locked()) {
      SelectionHandle new_sel = (*it)->get_selection(rect, layermask);
      selection->add(new_sel->begin(), new_sel->end());
    }
  return selection;
}

void SectorModel::raise(ObjectModelHandle object)
{ if (auto l = get_layer(object)) l->raise(object); }
void SectorModel::lower(ObjectModelHandle object)
{ if (auto l = get_layer(object)) l->lower(object); }
void SectorModel::raise_to_top(ObjectModelHandle object)
{ if (auto l = get_layer(object)) l->raise_to_top(object); }
void SectorModel::lower_to_bottom(ObjectModelHandle object)
{ if (auto l = get_layer(object)) l->lower_to_bottom(object); }

SnapData
SectorModel::snap_object(ObjectModelHandle object, std::set<ObjectModelHandle> const& ignore_objects) const
{
  SnapData snap_data;
  for (auto const& layer : m_layers)
    if (layer->is_visible())
      snap_data.merge(layer->snap_object(object, ignore_objects));
  return snap_data;
}

void
SectorModel::write(FileWriter& writer) const
{
  writer.begin_object("windstille-sector");
  writer.write("version", 3);
  writer.write("name", "");
  writer.write("ambient-color", ambient_color);
  writer.write("init-script", "init.nut");
  writer.begin_collection("timeline");
  m_timeline->write(writer);
  writer.end_collection();
  writer.begin_collection("navigation");
  nav_graph->write(writer);
  writer.end_collection();
  writer.begin_collection("layers");
  for (auto const& layer : m_layers) {
    writer.begin_object("layer");
    writer.write("name", layer->get_name());
    writer.write("visible", layer->is_visible());
    writer.write("locked", layer->is_locked());
    writer.begin_collection("objects");
    layer->write(writer);
    writer.end_collection();
    writer.end_object();
  }
  writer.end_collection();
  writer.end_object();
}

void SectorModel::set_all_visible(bool v)
{
  for (auto const& layer : m_layers) layer->set_visible(v);
  notify_layers_changed();
}

void SectorModel::set_all_locked(bool v)
{
  for (auto const& layer : m_layers) layer->set_locked(v);
  notify_layers_changed();
}

void
SectorModel::draw_content(SceneContext& sc)
{
  for (auto const& layer : m_layers)
    if (layer && layer->is_visible())
      for (auto obj = layer->begin(); obj != layer->end(); ++obj)
        (*obj)->draw_content(sc);
  for (auto const& node : nav_graph->get_nodes())
    node->draw_content(sc);
}

void SectorModel::delete_navgraph_edges(NavGraphNodeObjectModel&) {}

} // namespace windstille

/* EOF */
