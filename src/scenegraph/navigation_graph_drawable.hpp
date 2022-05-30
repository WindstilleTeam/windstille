// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_NAVIGATION_GRAPH_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_NAVIGATION_GRAPH_DRAWABLE_HPP

#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/scene_context.hpp>
#include <wstdisplay/scenegraph/drawable.hpp>

#include "navigation/navigation_graph.hpp"

namespace windstille {

class NavigationGraphDrawable : public wstdisplay::Drawable
{
private:
  NavigationGraph* m_navgraph;

public:
  NavigationGraphDrawable(NavigationGraph* navgraph)
    : Drawable(glm::vec2(), 1000.0f, glm::mat4(1.0)),
      m_navgraph(navgraph)
  {
    set_render_mask(wstdisplay::SceneContext::CONTROLMAP);
  }

  void render(wstdisplay::GraphicsContext& gc, unsigned int mask) override
  {
    glLineWidth(4.0f);
    m_navgraph->draw(gc);
    glLineWidth(1.0f);
  }

private:
  NavigationGraphDrawable(const NavigationGraphDrawable&);
  NavigationGraphDrawable& operator=(const NavigationGraphDrawable&);
};

} // namespace windstille

#endif

/* EOF */
