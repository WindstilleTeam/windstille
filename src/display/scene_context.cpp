/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <GL/glew.h>

#include "display/display.hpp"
#include "display/surface.hpp"
#include "display/opengl_state.hpp"

#include "display/scene_context.hpp"

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 4
#define BLURMAP_DIV  1

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 
  DrawingContext control; 

  unsigned int   render_mask;
  
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

  SceneContextImpl() 
    : color(),
      light(),
      highlight(),
      control(),
      render_mask(SceneContext::COLORMAP |
                  SceneContext::LIGHTMAP | 
                  SceneContext::HIGHLIGHTMAP | 
                  SceneContext::CONTROLMAP | 
                  SceneContext::LIGHTMAPSCREEN |
                  SceneContext::BLURMAP),
      framebuffers(0),
      //framebuffers(new Framebuffers()),
      lightmap(Display::get_width()/LIGHTMAP_DIV, Display::get_height()/LIGHTMAP_DIV)
  {
  }

  ~SceneContextImpl()
  {
  }
};

SceneContext::SceneContext()
  : impl(new SceneContextImpl())
{
}

SceneContext::~SceneContext()
{
}

DrawingContext&
SceneContext::color()
{
  return impl->color; 
}

DrawingContext&
SceneContext::light()
{ 
  return impl->light; 
}

DrawingContext&
SceneContext::highlight()
{ 
  return impl->highlight; 
}

DrawingContext&
SceneContext::control()
{
  return impl->control;
}

/** Translate the drawing context */
void
SceneContext::translate(float x, float y)
{
  impl->color.translate(x, y);
  impl->light.translate(x, y);
  impl->highlight.translate(x, y);
  impl->control.translate(x, y);
}

/** Set the rotation of the drawing context */
void
SceneContext::rotate(float angel, float x, float y, float z)
{
  impl->color.rotate(angel, x, y, z);
  impl->light.rotate(angel, x, y, z);
  impl->highlight.rotate(angel, x, y, z);
  impl->control.rotate(angel, x, y, z);
}

/** Set the scaling of the drawing context */
void
SceneContext::scale(float x, float y)
{
  impl->color.scale(x, y);
  impl->light.scale(x, y);
  impl->highlight.scale(x, y);
  impl->control.scale(x, y);
}

void
SceneContext::mult_modelview(const Matrix& matrix)
{
  impl->color.mult(matrix);
  impl->light.mult(matrix);
  impl->highlight.mult(matrix);
  impl->control.mult(matrix);
}

void
SceneContext::push_modelview()
{
  impl->color.push_modelview();
  impl->light.push_modelview();
  impl->highlight.push_modelview();
  impl->control.push_modelview();
}

void
SceneContext::pop_modelview()
{
  impl->color.pop_modelview();
  impl->light.pop_modelview();
  impl->highlight.pop_modelview();
  impl->control.pop_modelview();
}

void
SceneContext::reset_modelview()
{
  impl->color.reset_modelview();
  impl->light.reset_modelview();
  impl->highlight.reset_modelview();
  impl->control.reset_modelview();
}

void draw_disc(int count)
{
  float radius = static_cast<float>(count) * 2.0f;
  float minradius = 2.0f * static_cast<float>(count) - 164.0f;
  if (minradius < 0)
    minradius = 0;
  glClear(GL_DEPTH_BUFFER_BIT);

  int segments = 64;
  
  glBegin(GL_QUADS);
  for (int i = 0; i < segments; ++i)
    {
      float angel = 2.0f * math::pi / static_cast<float>(segments);

      float x1 =  sinf(angel * static_cast<float>(i)) * radius;
      float y1 = -cosf(angel * static_cast<float>(i)) * radius;

      float x2 =  sinf(angel * static_cast<float>(i+1)) * radius;
      float y2 = -cosf(angel * static_cast<float>(i+1)) * radius;

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glTexCoord2f(x1/512.0f+0.5f, y1/512.0f+0.5f);
      glVertex3f(x1+256, y1+256, 0);

      glTexCoord2f(x2/512.0f+0.5f, y2/512.0f+0.5f);
      glVertex3f(x2+256, y2+256, 0);

      float x3 =  sinf(angel * static_cast<float>(i)) * minradius;
      float y3 = -cosf(angel * static_cast<float>(i)) * minradius;

      float x4 =  sinf(angel * static_cast<float>(i+1)) * minradius;
      float y4 = -cosf(angel * static_cast<float>(i+1)) * minradius;

      glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
      glTexCoord2f(x4/512.0f+0.5f, y4/512.0f+0.5f);
      glVertex3f(x4+256, y4+256, 0);
      glTexCoord2f(x3/512.0f+0.5f, y3/512.0f+0.5f);
      glVertex3f(x3+256, y3+256, 0);
    }
  glEnd();
}

void
SceneContext::render_lightmap()
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
SceneContext::render_with_framebuffers()
{
  glClear(GL_DEPTH_BUFFER_BIT);
      
  if (impl->render_mask & LIGHTMAPSCREEN)
    {
      // Render the lightmap to the framebuffers->lightmap
      Display::push_framebuffer(impl->framebuffers->lightmap);
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glPushMatrix();
      glTranslatef(0.0f, static_cast<float>(Display::get_height() - (Display::get_height() / LIGHTMAP_DIV)), 0.0f);
      glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
      impl->light.render(*this);
      glPopMatrix();

      Display::pop_framebuffer();
    }

  if (impl->render_mask & COLORMAP)
    {
      // Render the colormap to the framebuffers->screen
      Display::push_framebuffer(impl->framebuffers->screen);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      impl->color.render(*this);
      Display::pop_framebuffer();
    }

  if (impl->render_mask & LIGHTMAP)
    { // Renders the lightmap to the screen
      Display::push_framebuffer(impl->framebuffers->screen);
      render_lightmap();
      Display::pop_framebuffer();
    }

  if (impl->render_mask & HIGHLIGHTMAP)
    {
      Display::push_framebuffer(impl->framebuffers->screen);
      impl->highlight.render(*this);
      Display::pop_framebuffer();
    }

  if (impl->render_mask & CONTROLMAP)
    impl->control.render(*this);

  if (1) 
    {
      // Render the screen framebuffer to the actual screen 
      OpenGLState state;

      Rectf uv(0.375f, 0.375f, 
               static_cast<float>(impl->framebuffers->screen.get_width())  + 0.375f,
               static_cast<float>(impl->framebuffers->screen.get_height()) + 0.375f);

      if (impl->render_mask & BLURMAP)
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
  impl->color.clear();
  impl->light.clear();
  impl->highlight.clear();
  impl->control.clear();
}

void
SceneContext::render_without_framebuffers()
{
  // Resize Lightmap, only needed in the editor, FIXME: move this into a 'set_size()' call
  if (impl->lightmap.get_width()  != Display::get_width()/LIGHTMAP_DIV ||
      impl->lightmap.get_height() != Display::get_height()/LIGHTMAP_DIV)
    {
      impl->lightmap = Surface(Display::get_width()/LIGHTMAP_DIV, Display::get_height()/LIGHTMAP_DIV);
    }

  if (impl->render_mask & LIGHTMAPSCREEN)
    {
      // Render the lightmap to the framebuffers->lightmap
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glPushMatrix();
      glTranslatef(0.0f, static_cast<float>(Display::get_height()) - static_cast<float>(Display::get_height()/LIGHTMAP_DIV), 0.0f);
      glScalef(1.0f / LIGHTMAP_DIV, 1.0f / LIGHTMAP_DIV, 1.0f);
      impl->light.render(*this);
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

  if (impl->render_mask & COLORMAP)
    {
      // Render the colormap to the framebuffers->screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      impl->color.render(*this);
    }

  if (impl->render_mask & LIGHTMAP)
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

  if (impl->render_mask & HIGHLIGHTMAP)
    {
      impl->highlight.render(*this);
    }  

  if (impl->render_mask & CONTROLMAP)
    impl->control.render(*this);

  // Clear all DrawingContexts
  impl->color.clear();
  impl->light.clear();
  impl->highlight.clear();
  impl->control.clear();
  
}

void
SceneContext::render()
{
  if (impl->framebuffers)
    {
      render_with_framebuffers();
    }
  else
    {
      render_without_framebuffers();
    }
}

void
SceneContext::set_render_mask(unsigned int mask)
{
  impl->render_mask = mask;
}

unsigned int
SceneContext::get_render_mask()
{
  return impl->render_mask;
}

DrawingContext&
SceneContext::get_layer(unsigned int type)
{
  switch(type)
    {
    case COLORMAP:
      return impl->color;

    case LIGHTMAP:
      return impl->light;

    case HIGHLIGHTMAP:
      return impl->highlight;

    default:
      assert(!"SceneContext::get_layer(): Unknown type");
      return impl->color; // never reached, but makes compiler happy
    }
}

void
SceneContext::eval(DrawingRequest* request)
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
