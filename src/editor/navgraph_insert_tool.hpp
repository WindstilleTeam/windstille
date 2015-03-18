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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_INSERT_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_INSERT_TOOL_HPP

#include <set>
#include <boost/shared_ptr.hpp>

#include "math/rect.hpp"
#include "editor/tool.hpp"
#include "navigation/navigation_graph.hpp"

class NavGraphNodeObjectModel;
class NavGraphEdgeObjectModel;

class NavgraphInsertTool : public Tool
{
private:
  Vector2f mouse_pos;

  enum {
    EDGE_MODE,   // create an edge between two nodes
    NO_MODE      // mode is determined by next click
  } mode;

  boost::shared_ptr<NavGraphNodeObjectModel> last_node;
  boost::shared_ptr<NavGraphNodeObjectModel> connection_node;

  boost::shared_ptr<NavGraphNodeObjectModel> mouse_over_node;
  boost::shared_ptr<NavGraphEdgeObjectModel> mouse_over_edge;

public:
  NavgraphInsertTool();

  void mouse_down(GdkEventButton* event, WindstilleWidget& wst);
  void mouse_move(GdkEventMotion* event, WindstilleWidget& wst);
  void mouse_up(GdkEventButton* event, WindstilleWidget& wst);

  void mouse_right_down(GdkEventButton* event, WindstilleWidget& wst);

  void draw(SceneContext& sc);

private:
  NavgraphInsertTool(const NavgraphInsertTool&);
  NavgraphInsertTool& operator=(const NavgraphInsertTool&);
};

#endif

/* EOF */
