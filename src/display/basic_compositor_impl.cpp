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
#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>

#include "display/graphic_context_state.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/scene_graph.hpp"

static const int LIGHTMAP_DIV = 4;

BasicCompositorImpl::BasicCompositorImpl(const Size& window, const Size& viewport) :
  CompositorImpl(window, viewport),
  m_lightmap(Surface::create(m_window.width  / LIGHTMAP_DIV,
                             m_window.height / LIGHTMAP_DIV))
{
}

void
BasicCompositorImpl::render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  // Resize Lightmap, only needed in the editor, FIXME: move this into a 'set_size()' call
  if (m_lightmap->get_width()  != m_window.width /LIGHTMAP_DIV ||
      m_lightmap->get_height() != m_window.height/LIGHTMAP_DIV)
  {
    m_lightmap = Surface::create(m_window.width / LIGHTMAP_DIV, m_window.height / LIGHTMAP_DIV);
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to the framebuffers->lightmap
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
    sc.light().render();
    glPopMatrix();

    if (sg)
    {
      glPushMatrix();
      glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
      glMultMatrixf(glm::value_ptr(gc_state.get_matrix()));
      sg->render(SceneContext::LIGHTMAP);
      glPopMatrix();
    }

    { // Copy lightmap to a texture
      OpenGLState state;

      state.bind_texture(m_lightmap->get_texture());
      state.activate();

      glCopyTexSubImage2D(GL_TEXTURE_2D,
                          0,    // mipmap level
                          0, 0, // xoffset, yoffset
                          0, // x
                          m_window.height - static_cast<GLsizei>(m_lightmap->get_height()), // y (OpenGL is upside down)
                          static_cast<GLsizei>(m_lightmap->get_width()),
                          static_cast<GLsizei>(m_lightmap->get_height()));
    }
  }

  if (sc.get_render_mask() & SceneContext::COLORMAP)
  {
    // Render the colormap to the framebuffers->screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sc.color().render();

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(glm::value_ptr(gc_state.get_matrix()));
      sg->render(SceneContext::COLORMAP);
      glPopMatrix();
    }
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAP)
  { // Renders the lightmap to the screen
    OpenGLState state;

    state.bind_texture(m_lightmap->get_texture());

    state.enable(GL_BLEND);
    state.set_blend_func(GL_DST_COLOR, GL_ZERO);
    state.activate();

    glBegin(GL_QUADS);

    glTexCoord2f(m_lightmap->get_uv().left, m_lightmap->get_uv().bottom);
    glVertex2i(0, 0);

    glTexCoord2f(m_lightmap->get_uv().right, m_lightmap->get_uv().bottom);
    glVertex2i(m_viewport.width, 0);

    glTexCoord2f(m_lightmap->get_uv().right, m_lightmap->get_uv().top);
    glVertex2i(m_viewport.width, m_viewport.height);

    glTexCoord2f(m_lightmap->get_uv().left, m_lightmap->get_uv().top);
    glVertex2i(0, m_viewport.height);

    glEnd();
  }

  if (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP)
  {
    sc.highlight().render();

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(glm::value_ptr(gc_state.get_matrix()));
      sg->render(SceneContext::HIGHLIGHTMAP);
      glPopMatrix();
    }
  }

  if (sc.get_render_mask() & SceneContext::CONTROLMAP)
  {
    sc.control().render();

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(glm::value_ptr(gc_state.get_matrix()));
      sg->render(SceneContext::CONTROLMAP);
      glPopMatrix();
    }
  }

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

/* EOF */
