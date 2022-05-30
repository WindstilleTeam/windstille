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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_NODE_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_NODE_OBJECT_MODEL_HPP

#include <memory>

#include <wstdisplay/fwd.hpp>

#include "editor/object_model.hpp"
#include "navigation/navigation_graph.hpp"

class FileReader;

class NavGraphNodeObjectModel : public ObjectModel
{
private:
  std::shared_ptr<wstdisplay::VertexArrayDrawable> m_drawable;

public:
  NavGraphNodeObjectModel(ReaderMapping const& reader);
  NavGraphNodeObjectModel(glm::vec2 const& pos);
  ~NavGraphNodeObjectModel() override;

  void add_to_scenegraph(wstdisplay::DrawableGroup& sg) override;
  void set_rel_pos(glm::vec2 const& rel_pos_) override;
  void sync_drawable();

  geom::frect get_bounding_box() const override;
  ObjectModelHandle clone() const override;
  void write(FileWriter& writer) const override;

  SnapData snap_to_object(geom::frect const& rect) const override;
  SnapData snap_to_grid(float grid_size) const override;

private:
  NavGraphNodeObjectModel(NavGraphNodeObjectModel const&);
  NavGraphNodeObjectModel& operator=(NavGraphNodeObjectModel const&);
};

#endif

/* EOF */
