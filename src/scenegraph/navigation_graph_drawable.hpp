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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_NAVIGATION_GRAPH_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_NAVIGATION_GRAPH_DRAWABLE_HPP

#include "display/display.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/drawable.hpp"

class NavigationGraph;

class NavigationGraphDrawable : public Drawable
{
private:
  NavigationGraph* m_navgraph;

public:
  NavigationGraphDrawable(NavigationGraph* navgraph)
    : Drawable(Vector2f(), 1000.0f, glm::mat4(1.0)),
      m_navgraph(navgraph)
  {
    set_render_mask(SceneContext::CONTROLMAP);
  }

  void render(unsigned int mask)
  {
    glLineWidth(4.0f);
    m_navgraph->draw();
    glLineWidth(1.0f);
  }

private:
  NavigationGraphDrawable(const NavigationGraphDrawable&);
  NavigationGraphDrawable& operator=(const NavigationGraphDrawable&);
};

#endif

/* EOF */
