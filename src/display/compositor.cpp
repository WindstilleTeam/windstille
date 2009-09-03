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

#include "display/compositor.hpp"

#include <GL/glew.h>

#include "display/display.hpp"
#include "scenegraph/drawable.hpp"
#include "display/graphic_context_state.hpp"
#include "display/framebuffer.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "math/rect.hpp"
#include "scenegraph/scene_graph.hpp"

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 4

struct CompositorImpl
{
  struct Framebuffers 
  {
    Framebuffer screen;
    Framebuffer lightmap;   

    Framebuffers(const Size& window) 
      : screen  (GL_TEXTURE_RECTANGLE_ARB, window.width, window.height),
        lightmap(GL_TEXTURE_RECTANGLE_ARB, window.width / LIGHTMAP_DIV, window.height / LIGHTMAP_DIV)
    {
    }
  };

  Size m_window;
  Size m_viewport; 
  boost::scoped_ptr<Framebuffers> framebuffers;
  Surface lightmap;

  CompositorImpl(const Size& window, const Size& viewport)
    : m_window(window),
      m_viewport(viewport),
      framebuffers(0),
      lightmap(m_window.width  / LIGHTMAP_DIV,
               m_window.height / LIGHTMAP_DIV)
  {
      if (GLEW_EXT_framebuffer_object) 
      {
        std::cout  << "Display:: framebuffer_object extension is supported" << std::endl;
        framebuffers.reset(new Framebuffers(m_window));
      }
      else
      {
        std::cout  << "Display:: framebuffer_object extension is not supported" << std::endl;
      }
  }
};

Compositor::Compositor(const Size& window, const Size& viewport)
  : impl(new CompositorImpl(window, viewport))
{
}

Compositor::~Compositor()
{
}

void
Compositor::render_lightmap(SceneContext& /*sc*/, SceneGraph* /*sg*/)
{
  Rectf uv(0, 0,
           static_cast<float>(impl->framebuffers->lightmap.get_width()), 
           static_cast<float>(impl->framebuffers->lightmap.get_height()));

  OpenGLState state;

  state.bind_texture(impl->framebuffers->lightmap.get_texture());
      
  state.enable(GL_BLEND);
  state.set_blend_func(GL_DST_COLOR, GL_ZERO); // multiple the lightmap with the screen
  state.activate();

  glBegin(GL_QUADS);
  {
    glTexCoord2f(uv.left, uv.bottom);
    glVertex2i(0, 0);

    glTexCoord2f(uv.right, uv.bottom);
    glVertex2i(impl->m_viewport.width, 0);

    glTexCoord2f(uv.right, uv.top);
    glVertex2i(impl->m_viewport.width, impl->m_viewport.height);

    glTexCoord2f(uv.left, uv.top);
    glVertex2i(0, impl->m_viewport.height);
  }
  glEnd();
}

void
Compositor::render_with_framebuffers(SceneContext& sc, SceneGraph* sg,
                                     const GraphicContextState& gc_state)
{
  glClear(GL_DEPTH_BUFFER_BIT);
      
  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to framebuffers->lightmap
    Display::push_framebuffer(impl->framebuffers->lightmap);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, static_cast<float>(impl->m_viewport.height - (impl->m_viewport.height / LIGHTMAP_DIV)), 0.0f);
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV);
    sc.light().render(*this);

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(gc_state.get_matrix().matrix);
      sg->draw(SceneContext::LIGHTMAP);
      glPopMatrix();
    }

    glPopMatrix();
    
    Display::pop_framebuffer();
  }

  { // Render the main screen
    Display::push_framebuffer(impl->framebuffers->screen);

    if (sc.get_render_mask() & SceneContext::COLORMAP)
    {
      // Render the colormap to framebuffers->screen
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      sc.color().render(*this);

      if (sg)
      {
        glPushMatrix();
        glMultMatrixf(gc_state.get_matrix().matrix);
        sg->draw(SceneContext::COLORMAP);
        glPopMatrix();
      }
    }

    if (sc.get_render_mask() & SceneContext::LIGHTMAP)
    { // Renders the lightmap to the screen
      render_lightmap(sc, sg);
    }

    if (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP)
    {
      sc.highlight().render(*this);

      if (sg)
      {
        glPushMatrix();
        glMultMatrixf(gc_state.get_matrix().matrix);
        sg->draw(SceneContext::HIGHLIGHTMAP);
        glPopMatrix();
      }
    }

    if (sc.get_render_mask() & SceneContext::CONTROLMAP)
    {
      sc.control().render(*this);

      if (sg)
      {
        glPushMatrix();
        glMultMatrixf(gc_state.get_matrix().matrix);
        sg->draw(SceneContext::CONTROLMAP);
        glPopMatrix();
      }
    }

    Display::pop_framebuffer();
  }

  if (1) 
  {
    // Render the screen framebuffer to the actual screen 
    OpenGLState state;

    Rectf uv(0, 0, 
             static_cast<float>(impl->framebuffers->screen.get_width()),
             static_cast<float>(impl->framebuffers->screen.get_height()));

    state.bind_texture(impl->framebuffers->screen.get_texture(), 0);

    state.activate();

    glBegin(GL_QUADS);

    glTexCoord2f(uv.left, uv.bottom);
    glVertex2f(0, 0);

    glTexCoord2f(uv.right, uv.bottom);
    glVertex2f(static_cast<float>(impl->m_viewport.width), 0);

    glTexCoord2f(uv.right, uv.top);
    glVertex2f(static_cast<float>(impl->m_viewport.width), static_cast<float>(impl->m_viewport.height));

    glTexCoord2f(uv.left, uv.top);
    glVertex2f(0.0f, static_cast<float>(impl->m_viewport.height));

    glEnd();
  }

  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear();
}

void
Compositor::render_without_framebuffers(SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state)
{
  // Resize Lightmap, only needed in the editor, FIXME: move this into a 'set_size()' call
  if (impl->lightmap.get_width()  != impl->m_window.width /LIGHTMAP_DIV ||
      impl->lightmap.get_height() != impl->m_window.height/LIGHTMAP_DIV)
  {
    impl->lightmap = Surface(impl->m_window.width / LIGHTMAP_DIV, impl->m_window.height / LIGHTMAP_DIV);
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to the framebuffers->lightmap
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, static_cast<float>(impl->m_viewport.height) - static_cast<float>(impl->m_viewport.height/LIGHTMAP_DIV), 0.0f);
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
    sc.light().render(*this);
    glPopMatrix();

    if (sg)
    {
      glPushMatrix();
      glTranslatef(0.0f, static_cast<float>(impl->m_viewport.height) - static_cast<float>(impl->m_viewport.height/LIGHTMAP_DIV), 0.0f);
      glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
      glMultMatrixf(gc_state.get_matrix().matrix);
      sg->draw(SceneContext::LIGHTMAP);
      glPopMatrix();
    }

    { // Copy lightmap to a texture
      OpenGLState state;
        
      // Weird y-pos is needed since OpenGL is upside down when it comes to y-coordinate
      state.bind_texture(impl->lightmap.get_texture());
      state.activate();

      glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                          0, 0, 
                          0, 0, //impl->m_window.height - impl->lightmap.get_height(),
                          static_cast<GLsizei>(impl->lightmap.get_width()), 
                          static_cast<GLsizei>(impl->lightmap.get_height()));
    }
  }

  if (sc.get_render_mask() & SceneContext::COLORMAP)
  {
    // Render the colormap to the framebuffers->screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sc.color().render(*this);

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(gc_state.get_matrix().matrix);
      sg->draw(SceneContext::COLORMAP);
      glPopMatrix();
    }
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAP)
  { // Renders the lightmap to the screen     
    OpenGLState state;

    Rectf uv = impl->lightmap.get_uv();

    state.bind_texture(impl->lightmap.get_texture());

    state.enable(GL_BLEND);
    state.set_blend_func(GL_DST_COLOR, GL_ZERO);
    state.activate();

    glBegin(GL_QUADS);

    glTexCoord2f(uv.left, uv.bottom);
    glVertex2f(0, 0);

    glTexCoord2f(uv.right, uv.bottom);
    glVertex2f(impl->lightmap.get_width() * LIGHTMAP_DIV, 0);

    glTexCoord2f(uv.right, uv.top);
    glVertex2f(impl->lightmap.get_width()  * LIGHTMAP_DIV,
               impl->lightmap.get_height() * LIGHTMAP_DIV);

    glTexCoord2f(uv.left, uv.top);
    glVertex2f(0, impl->lightmap.get_height() * LIGHTMAP_DIV);

    glEnd();
  }

  if (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP)
  {
    sc.highlight().render(*this);

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(gc_state.get_matrix().matrix);
      sg->draw(SceneContext::HIGHLIGHTMAP);
      glPopMatrix();
    }
  }

  if (sc.get_render_mask() & SceneContext::CONTROLMAP)
    sc.control().render(*this);
  
  // Clear all DrawingContexts
  sc.color().clear();
  sc.light().clear();
  sc.highlight().clear();
  sc.control().clear(); 
}

void
Compositor::render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& state)
{
  if (impl->framebuffers)
  {
    render_with_framebuffers(sc, sg, state);
  }
  else
  {
    render_without_framebuffers(sc, sg, state);
  }
}

/* EOF */
