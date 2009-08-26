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

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 
  DrawingContext control; 

  unsigned int   render_mask;

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
                  SceneContext::BLURMAP)
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

/* EOF */
