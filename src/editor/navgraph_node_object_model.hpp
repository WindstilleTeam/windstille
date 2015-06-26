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

#include <boost/shared_ptr.hpp>

#include "editor/object_model.hpp"
#include "navigation/navigation_graph.hpp"

class VertexArrayDrawable;
class FileReader;

class NavGraphNodeObjectModel : public ObjectModel
{
private:
  boost::shared_ptr<VertexArrayDrawable> m_drawable;

public:
  NavGraphNodeObjectModel(const FileReader& reader);
  NavGraphNodeObjectModel(const Vector2f& pos);
  ~NavGraphNodeObjectModel();

  void add_to_scenegraph(DrawableGroup& sg);
  void set_rel_pos(const Vector2f& rel_pos_);
  void sync_drawable();

  Rectf get_bounding_box() const;
  ObjectModelHandle clone() const;
  void write(FileWriter& writer) const;

  SnapData snap_to_object(const Rectf& rect) const;
  SnapData snap_to_grid(float grid_size) const;

private:
  NavGraphNodeObjectModel(const NavGraphNodeObjectModel&);
  NavGraphNodeObjectModel& operator=(const NavGraphNodeObjectModel&);
};

#endif

/* EOF */
