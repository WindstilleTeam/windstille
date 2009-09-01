/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_SCREEN_NAVIGATION_TEST_HPP
#define HEADER_WINDSTILLE_SCREEN_NAVIGATION_TEST_HPP

#include <boost/scoped_ptr.hpp>

#include "math/vector2f.hpp"
#include "navigation/navigation_graph.hpp"
#include "screen/screen.hpp"

class NavigationGraph;
class EdgePosition;
class Edge;
class Node;

/** */
class NavigationTest : public Screen
{
private:
  Vector2f cursor;
  Vector2f stick;
  Vector2f player;
  Vector2f old_player;
  
  boost::scoped_ptr<NavigationGraph> graph;
  boost::scoped_ptr<EdgePosition> connection;

  EdgeHandle selected_edge;
  NodeHandle selected_node;

  NodeHandle node_to_connect;

public:
  NavigationTest();

  void draw();
  void update(float delta, const Controller& controller);

private:
  NavigationTest (const NavigationTest&);
  NavigationTest& operator= (const NavigationTest&);
};

#endif

/* EOF */
