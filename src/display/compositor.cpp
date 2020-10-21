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

#include "display/compositor.hpp"

#include <glm/ext.hpp>

#include "display/assert_gl.hpp"
#include "display/graphic_context_state.hpp"
#include "display/graphics_context.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

static const int LIGHTMAP_DIV = 4;

Compositor::Compositor(geom::isize const& framebuffer_size,
                                                     geom::isize const& viewport_size) :
  m_framebuffer_size(framebuffer_size),
  m_viewport_size(viewport_size),
  m_screen(Framebuffer::create_with_texture(GL_TEXTURE_2D, framebuffer_size)),
  m_lightmap(Framebuffer::create_with_texture(GL_TEXTURE_2D,
                                              {std::max(framebuffer_size.width() / LIGHTMAP_DIV, 1),
                                               std::max(framebuffer_size.height() / LIGHTMAP_DIV, 1)}))
{
  assert_gl();
}

void
Compositor::render_lightmap(GraphicsContext& gc)
{
  VertexArrayDrawable va;

  va.set_texture(m_lightmap->get_texture());
  va.set_blend_func(GL_DST_COLOR, GL_ZERO); // multiply the lightmap with the screen

  float const vw = 1.0f;
  float const vh = 1.0f;

  va.set_mode(GL_TRIANGLE_FAN);

  va.texcoord(0, vh);
  va.vertex(0, 0);

  va.texcoord(vw, vh);
  va.vertex(m_viewport_size.width(), 0);

  va.texcoord(vw, 0);
  va.vertex(m_viewport_size.width(), m_viewport_size.height());

  va.texcoord(0, 0);
  va.vertex(0, m_viewport_size.height());

  va.render(gc);
}

void
Compositor::render(GraphicsContext& gc, SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to framebuffers->lightmap
    gc.push_framebuffer(m_lightmap);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gc.push_matrix();
    gc.translate(0.0f, static_cast<float>(m_viewport_size.height() - (m_viewport_size.height() / LIGHTMAP_DIV)), 0.0f);
    gc.scale(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV);

    sc.light().render(gc);

    if (sg)
    {
      gc.push_matrix();
      gc.mult_matrix(gc_state.get_matrix());
      sg->render(gc, SceneContext::LIGHTMAP);
      gc.pop_matrix();
    }

    gc.pop_matrix();

    gc.pop_framebuffer();
  }

  { // Render the main screen
    gc.push_framebuffer(m_screen);

    if (sc.get_render_mask() & SceneContext::COLORMAP)
    {
      // Render the colormap to framebuffers->screen
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      sc.color().render(gc);

      if (sg)
      {
        gc.push_matrix();
        gc.mult_matrix(gc_state.get_matrix());
        sg->render(gc, SceneContext::COLORMAP);
        gc.pop_matrix();
      }
    }

    if (sc.get_render_mask() & SceneContext::LIGHTMAP)
    { // Renders the lightmap to the screen
      render_lightmap(gc);
    }

    if (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP)
    {
      sc.highlight().render(gc);

      if (sg)
      {
        gc.push_matrix();
        gc.mult_matrix(gc_state.get_matrix());
        sg->render(gc, SceneContext::HIGHLIGHTMAP);
        gc.pop_matrix();
      }
    }

    if (sc.get_render_mask() & SceneContext::CONTROLMAP)
    {
      sc.control().render(gc);

      if (sg)
      {
        gc.push_matrix();
        gc.mult_matrix(gc_state.get_matrix());
        sg->render(gc, SceneContext::CONTROLMAP);
        gc.pop_matrix();
      }
    }

    gc.pop_framebuffer();
  }

  { // Render the screen framebuffer to the actual screen
    VertexArrayDrawable va;

    va.set_texture(m_screen->get_texture() /*, 0*/);

    float const vw = 1.0f;
    float const vh = 1.0f;

    va.set_mode(GL_TRIANGLE_FAN);
    {
      va.texcoord(0, vh);
      va.vertex(0, 0);

      va.texcoord(vw, vh);
      va.vertex(m_viewport_size.width(), 0);

      va.texcoord(vw, 0);
      va.vertex(m_viewport_size.width(), m_viewport_size.height());

      va.texcoord(0, 0);
      va.vertex(0, m_viewport_size.height());
    }
    va.render(gc);
  }

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

geom::isize
Compositor::get_framebuffer_size() const
{
  return m_framebuffer_size;
}

geom::isize
Compositor::get_viewport_size() const
{
  return m_viewport_size;
}

/* EOF */
