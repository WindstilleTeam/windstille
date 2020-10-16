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

#include "display/framebuffer_compositor_impl.hpp"

#include <glm/ext.hpp>

#include "display/graphics_context.hpp"
#include "display/graphic_context_state.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

static const int LIGHTMAP_DIV = 4;

FramebufferCompositorImpl::FramebufferCompositorImpl(const geom::isize& window, const geom::isize& viewport) :
  CompositorImpl(window, viewport),
  m_screen  (Framebuffer::create_with_texture(GL_TEXTURE_2D, window.width(), window.height())),
  m_lightmap(Framebuffer::create_with_texture(GL_TEXTURE_2D, window.width() / LIGHTMAP_DIV, window.height() / LIGHTMAP_DIV))
{
}

void
FramebufferCompositorImpl::render_lightmap(GraphicsContext& gc, SceneContext& /*sc*/, SceneGraph* /*sg*/)
{
  VertexArrayDrawable va;

  va.set_texture(m_lightmap->get_texture());
  va.set_blend_func(GL_DST_COLOR, GL_ZERO); // multiply the lightmap with the screen

  va.set_mode(GL_QUADS);
  {
    va.texcoord(0, 1);
    va.vertex(0, 0);

    va.texcoord(1, 1);
    va.vertex(m_viewport.width(), 0);

    va.texcoord(1, 0);
    va.vertex(m_viewport.width(), m_viewport.height());

    va.texcoord(0, 0);
    va.vertex(0, m_viewport.height());
  }
  va.render(gc);
}

void
FramebufferCompositorImpl::render(GraphicsContext& gc, SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to framebuffers->lightmap
    gc.push_framebuffer(m_lightmap);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gc.push_matrix();
    gc.translate(0.0f, static_cast<float>(m_viewport.height() - (m_viewport.height() / LIGHTMAP_DIV)), 0.0f);
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
      render_lightmap(gc, sc, sg);
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

  if (1)
  {
    // Render the screen framebuffer to the actual screen
    VertexArrayDrawable va;

    va.set_texture(m_screen->get_texture() /*, 0*/);

    va.set_mode(GL_QUADS);
    {
      va.texcoord(0, 1);
      va.vertex(0, 0);

      va.texcoord(1, 1);
      va.vertex(m_viewport.width(), 0);

      va.texcoord(1, 0);
      va.vertex(m_viewport.width(), m_viewport.height());

      va.texcoord(0, 0);
      va.vertex(0, m_viewport.height());
    }
    va.render(gc);
  }

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

/* EOF */
