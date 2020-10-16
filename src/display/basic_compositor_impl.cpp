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

#include "display/basic_compositor_impl.hpp"

#include <GL/glew.h>
#include <glm/ext.hpp>

#include "display/graphic_context_state.hpp"
#include "display/opengl_state.hpp"
#include "display/graphics_context.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

static const int LIGHTMAP_DIV = 4;

BasicCompositorImpl::BasicCompositorImpl(const geom::isize& window, const geom::isize& viewport) :
  CompositorImpl(window, viewport),
  m_lightmap(Surface::create(m_window.width()  / LIGHTMAP_DIV,
                             m_window.height() / LIGHTMAP_DIV))
{
}

void
BasicCompositorImpl::render(GraphicsContext& gc, SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  // Resize Lightmap, only needed in the editor, FIXME: move this into a 'set_size()' call
  if (m_lightmap->get_width()  != static_cast<float>(m_window.width()  / LIGHTMAP_DIV) ||
      m_lightmap->get_height() != static_cast<float>(m_window.height() / LIGHTMAP_DIV))
  {
    m_lightmap = Surface::create(m_window.width() / LIGHTMAP_DIV, m_window.height() / LIGHTMAP_DIV);
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to the framebuffers->lightmap
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gc.push_matrix();
    gc.scale(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
    sc.light().render(gc);
    gc.pop_matrix();

    if (sg)
    {
      gc.push_matrix();
      gc.scale(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
      gc.mult_matrix(gc_state.get_matrix());
      sg->render(gc, SceneContext::LIGHTMAP);
      gc.pop_matrix();
    }

    { // Copy lightmap to a texture
      OpenGLState state;

      state.bind_texture(m_lightmap->get_texture());
      state.activate();

      glCopyTexSubImage2D(GL_TEXTURE_2D,
                          0,    // mipmap level
                          0, 0, // xoffset, yoffset
                          0, // x
                          m_window.height() - static_cast<GLsizei>(m_lightmap->get_height()), // y (OpenGL is upside down)
                          static_cast<GLsizei>(m_lightmap->get_width()),
                          static_cast<GLsizei>(m_lightmap->get_height()));
    }
  }

  if (sc.get_render_mask() & SceneContext::COLORMAP)
  {
    // Render the colormap to the framebuffers->screen
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
    VertexArrayDrawable va;

    va.set_texture(m_lightmap->get_texture());

    va.set_blend_func(GL_DST_COLOR, GL_ZERO);

    va.set_mode(GL_QUADS);

    va.texcoord(m_lightmap->get_uv().left(), m_lightmap->get_uv().bottom());
    va.vertex(0, 0);

    va.texcoord(m_lightmap->get_uv().right(), m_lightmap->get_uv().bottom());
    va.vertex(m_viewport.width(), 0);

    va.texcoord(m_lightmap->get_uv().right(), m_lightmap->get_uv().top());
    va.vertex(m_viewport.width(), m_viewport.height());

    va.texcoord(m_lightmap->get_uv().left(), m_lightmap->get_uv().top());
    va.vertex(0, m_viewport.height());

    va.render(gc);
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

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

/* EOF */
