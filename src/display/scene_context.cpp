/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "display/graphics_context.hpp"
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
                  SceneContext::LIGHTMAPSCREEN)
  {
  }

  ~SceneContextImpl()
  {
  }
};

SceneContext::SceneContext() :
  impl(new SceneContextImpl())
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
SceneContext::mult_modelview(const glm::mat4& matrix)
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

/* EOF */
