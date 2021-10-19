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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_EDGE_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_EDGE_OBJECT_MODEL_HPP

#include <wstdisplay/fwd.hpp>

#include "navigation/navigation_graph.hpp"
#include "editor/object_model.hpp"

class NavGraphNodeObjectModel;

class NavGraphEdgeObjectModel : public ObjectModel
{
private:
  std::shared_ptr<NavGraphNodeObjectModel> m_lhs;
  std::shared_ptr<NavGraphNodeObjectModel> m_rhs;
  std::shared_ptr<wstdisplay::VertexArrayDrawable> m_drawable;

public:
  NavGraphEdgeObjectModel(std::shared_ptr<NavGraphNodeObjectModel> lhs,
                          std::shared_ptr<NavGraphNodeObjectModel> rhs);
  ~NavGraphEdgeObjectModel() override;

  void add_to_scenegraph(wstdisplay::DrawableGroup& sg) override;
  void update(float delta) override;

  geom::frect get_bounding_box() const override;
  ObjectModelHandle clone() const override;
  void write(FileWriter& writer) const override;
  void write_real(FileWriter& writer) const;

  bool is_snappable() const override { return false; }

  std::shared_ptr<NavGraphNodeObjectModel> get_lhs() const { return m_lhs; }
  std::shared_ptr<NavGraphNodeObjectModel> get_rhs() const { return m_rhs; }

private:
  NavGraphEdgeObjectModel(const NavGraphEdgeObjectModel&);
  NavGraphEdgeObjectModel& operator=(const NavGraphEdgeObjectModel&);
};

#endif

/* EOF */
