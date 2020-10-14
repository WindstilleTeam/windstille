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

#include "display/display.hpp"
#include "display/graphic_context_state.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "scenegraph/scene_graph.hpp"

static const int LIGHTMAP_DIV = 4;

FramebufferCompositorImpl::FramebufferCompositorImpl(const geom::isize& window, const geom::isize& viewport) :
  CompositorImpl(window, viewport),
  m_screen  (Framebuffer::create_with_texture(GL_TEXTURE_2D, window.width(), window.height())),
  m_lightmap(Framebuffer::create_with_texture(GL_TEXTURE_2D, window.width() / LIGHTMAP_DIV, window.height() / LIGHTMAP_DIV))
{
}

void
FramebufferCompositorImpl::render_lightmap(SceneContext& /*sc*/, SceneGraph* /*sg*/)
{
  OpenGLState state;

  state.bind_texture(m_lightmap->get_texture());

  state.enable(GL_BLEND);
  state.set_blend_func(GL_DST_COLOR, GL_ZERO); // multiply the lightmap with the screen
  state.activate();

  glBegin(GL_QUADS);
  {
    glTexCoord2i(0, 1);
    glVertex2i(0, 0);

    glTexCoord2i(1, 1);
    glVertex2i(m_viewport.width(), 0);

    glTexCoord2i(1, 0);
    glVertex2i(m_viewport.width(), m_viewport.height());

    glTexCoord2i(0, 0);
    glVertex2i(0, m_viewport.height());
  }
  glEnd();
}

void
FramebufferCompositorImpl::render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to framebuffers->lightmap
    Display::push_framebuffer(m_lightmap);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, static_cast<float>(m_viewport.height() - (m_viewport.height() / LIGHTMAP_DIV)), 0.0f);
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV);

    sc.light().render();

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(glm::value_ptr(gc_state.get_matrix()));
      sg->render(SceneContext::LIGHTMAP);
      glPopMatrix();
    }

    glPopMatrix();

    Display::pop_framebuffer();
  }

  { // Render the main screen
    Display::push_framebuffer(m_screen);

    if (sc.get_render_mask() & SceneContext::COLORMAP)
    {
      // Render the colormap to framebuffers->screen
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
      render_lightmap(sc, sg);
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

    Display::pop_framebuffer();
  }

  if (1)
  {
    // Render the screen framebuffer to the actual screen
    OpenGLState state;

    state.bind_texture(m_screen->get_texture(), 0);

    state.activate();

    glBegin(GL_QUADS);
    {
      glTexCoord2i(0, 1);
      glVertex2i(0, 0);

      glTexCoord2i(1, 1);
      glVertex2i(m_viewport.width(), 0);

      glTexCoord2i(1, 0);
      glVertex2i(m_viewport.width(), m_viewport.height());

      glTexCoord2i(0, 0);
      glVertex2i(0.0f, m_viewport.height());
    }
    glEnd();
  }

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

/* EOF */
