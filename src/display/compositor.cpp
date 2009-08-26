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
#include "display/drawing_request.hpp"
#include "display/graphic_context_state.hpp"
#include "display/framebuffer.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "math/rect.hpp"
#include "scenegraph/scene_graph.hpp"

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 4
#define BLURMAP_DIV  1

struct CompositorImpl
{
  struct Framebuffers 
  {
    Framebuffer screen;
    Framebuffer tmp;
    Framebuffer lightmap;   

    Framebuffers() 
      : screen  (GL_TEXTURE_RECTANGLE_ARB, Display::get_width(), Display::get_height()),
        tmp     (GL_TEXTURE_RECTANGLE_ARB, Display::get_width(), Display::get_height()),
        lightmap(GL_TEXTURE_RECTANGLE_ARB, Display::get_width()/LIGHTMAP_DIV, Display::get_height()/LIGHTMAP_DIV)
    {
    }
  };
  
  boost::scoped_ptr<Framebuffers> framebuffers;
  Surface lightmap;

  CompositorImpl()
    : framebuffers(0),
      //framebuffers(new Framebuffers()),
      lightmap(Display::get_width()  / LIGHTMAP_DIV,
               Display::get_height() / LIGHTMAP_DIV)
  {}
};

Compositor::Compositor()
  : impl(new CompositorImpl())
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

  glTexCoord2f(uv.left, uv.bottom);
  glVertex2f(0, 0);

  glTexCoord2f(uv.right, uv.bottom);
  glVertex2f(static_cast<float>(impl->framebuffers->lightmap.get_width() * LIGHTMAP_DIV), 0.0f);

  glTexCoord2f(uv.right, uv.top);
  glVertex2f(static_cast<float>(impl->framebuffers->lightmap.get_width()  * LIGHTMAP_DIV),
             static_cast<float>(impl->framebuffers->lightmap.get_height() * LIGHTMAP_DIV));

  glTexCoord2f(uv.left, uv.top);
  glVertex2f(0.0f, static_cast<float>(impl->framebuffers->lightmap.get_height() * LIGHTMAP_DIV));

  glEnd();
}

void
Compositor::render_with_framebuffers(SceneContext& sc, SceneGraph* sg)
{
  glClear(GL_DEPTH_BUFFER_BIT);
      
  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to the framebuffers->lightmap
    Display::push_framebuffer(impl->framebuffers->lightmap);
      
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, static_cast<float>(Display::get_height() - (Display::get_height() / LIGHTMAP_DIV)), 0.0f);
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
    sc.light().render(*this);
    glPopMatrix();

    Display::pop_framebuffer();
  }

  if (sc.get_render_mask() & SceneContext::COLORMAP)
  {
    // Render the colormap to the framebuffers->screen
    Display::push_framebuffer(impl->framebuffers->screen);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sc.color().render(*this);
    Display::pop_framebuffer();
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAP)
  { // Renders the lightmap to the screen
    Display::push_framebuffer(impl->framebuffers->screen);
    render_lightmap(sc, sg);
    Display::pop_framebuffer();
  }

  if (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP)
  {
    Display::push_framebuffer(impl->framebuffers->screen);
    sc.highlight().render(*this);
    Display::pop_framebuffer();
  }

  if (sc.get_render_mask() & SceneContext::CONTROLMAP)
    sc.control().render(*this);

  if (1) 
  {
    // Render the screen framebuffer to the actual screen 
    OpenGLState state;

    Rectf uv(0.375f, 0.375f, 
             static_cast<float>(impl->framebuffers->screen.get_width())  + 0.375f,
             static_cast<float>(impl->framebuffers->screen.get_height()) + 0.375f);

    if (sc.get_render_mask() & SceneContext::BLURMAP)
      state.bind_texture(impl->framebuffers->screen.get_texture(), 0);
    else
      state.bind_texture(impl->framebuffers->tmp.get_texture(), 0);

    state.activate();

    glBegin(GL_QUADS);

    glTexCoord2f(uv.left, uv.bottom);
    glVertex2f(0, 0);

    glTexCoord2f(uv.right, uv.bottom);
    glVertex2f(static_cast<float>(Display::get_width()), 0);

    glTexCoord2f(uv.right, uv.top);
    glVertex2f(static_cast<float>(Display::get_width()), static_cast<float>(Display::get_height()));

    glTexCoord2f(uv.left, uv.top);
    glVertex2f(0.0f, static_cast<float>(Display::get_height()));

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
  if (impl->lightmap.get_width()  != Display::get_width()/LIGHTMAP_DIV ||
      impl->lightmap.get_height() != Display::get_height()/LIGHTMAP_DIV)
  {
    impl->lightmap = Surface(Display::get_width()/LIGHTMAP_DIV, Display::get_height()/LIGHTMAP_DIV);
  }

  if (sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN)
  {
    // Render the lightmap to the framebuffers->lightmap
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, static_cast<float>(Display::get_height()) - static_cast<float>(Display::get_height()/LIGHTMAP_DIV), 0.0f);
    glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
    sc.light().render(*this);
    glPopMatrix();

    { // Copy lightmap to a texture
      OpenGLState state;
        
      // Weird y-pos is needed since OpenGL is upside down when it comes to y-coordinate
      state.bind_texture(impl->lightmap.get_texture());
      state.activate();

      glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                          0, 0, 
                          0, 0, //Display::get_height() - impl->lightmap.get_height(),
                          static_cast<GLsizei>(impl->lightmap.get_width()), 
                          static_cast<GLsizei>(impl->lightmap.get_height()));
    }
  }

  if (sc.get_render_mask() & SceneContext::COLORMAP)
  {
    // Render the colormap to the framebuffers->screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sc.color().render(*this);

    if (sg)
    {
      glPushMatrix();
      glMultMatrixf(gc_state.get_matrix().matrix);
      sg->draw(Texture());
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
    render_with_framebuffers(sc, sg);
  }
  else
  {
    render_without_framebuffers(sc, sg, state);
  }
}

void
Compositor::eval(DrawingRequest* request)
{
  if (impl->framebuffers && request->needs_prepare())
    {
      Display::push_framebuffer(impl->framebuffers->tmp);
      request->prepare(impl->framebuffers->screen.get_texture());
      Display::pop_framebuffer();
      
      Display::push_framebuffer(impl->framebuffers->screen);
      request->draw(impl->framebuffers->tmp.get_texture());
      Display::pop_framebuffer();
    }
  else
    {
      request->draw(Texture());
    }
}

/* EOF */
