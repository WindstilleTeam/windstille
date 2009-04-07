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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_TOOL_HPP

#include <set>

#include "math/rect.hpp"
#include "tool.hpp"
#include "navigation/navigation_graph.hpp"

class NavgraphTool : public Tool
{
private:
  Vector2f click_pos;
  Rectf    rect;

  enum { 
    SELECT_MODE, // select multiple nodes at once
    DRAG_MODE,   // move nodes around
    EDGE_MODE,   // create an edge between two nodes
    NO_MODE      // mode is determined by next click
  } mode;

  std::set<NodeHandle> selection;

public:
  NavgraphTool();

  bool mouse_down (GdkEventButton* event, WindstilleWidget& wst);
  bool mouse_move(GdkEventMotion* event, WindstilleWidget& wst);
  bool mouse_up(GdkEventButton* event, WindstilleWidget& wst);
  
  void draw(SceneContext& sc);

private:
  NavgraphTool(const NavgraphTool&);
  NavgraphTool& operator=(const NavgraphTool&);
};

#endif

/* EOF */
