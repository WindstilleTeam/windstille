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

#include "display/drawing_context.hpp"

#include <GL/glew.h>

#include "display/compositor.hpp"
#include "display/display.hpp"
#include "display/drawing_parameters.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "font/fonts.hpp"
#include "math/vector3.hpp"
#include "math/line.hpp"
#include "sprite2d/sprite.hpp"

#include "scenegraph/control_drawable.hpp"
#include "scenegraph/fill_screen_drawable.hpp"
#include "scenegraph/fill_screen_pattern_drawable.hpp"
#include "scenegraph/surface_drawable.hpp"
#include "scenegraph/surface_quad_drawable.hpp"
#include "scenegraph/text_drawable.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

struct DrawablesSorter
{
  bool operator()(Drawable* a, Drawable* b) {
    return a->get_z_pos() < b->get_z_pos();
  }
};

DrawingContext::DrawingContext()
  : drawingrequests(),
    modelview_stack()
{
  modelview_stack.push_back(Matrix::identity());
}

DrawingContext::~DrawingContext()
{
  clear();
}

void
DrawingContext::render(Compositor& /*comp*/)
{
  std::stable_sort(drawingrequests.begin(), drawingrequests.end(), DrawablesSorter());
  
  for(Drawables::iterator i = drawingrequests.begin(); i != drawingrequests.end(); ++i)
    {
      (*i)->draw();
    }
}

void
DrawingContext::clear()
{
  for(Drawables::iterator i = drawingrequests.begin(); i != drawingrequests.end(); ++i)
    {
      delete *i;
    }
  drawingrequests.clear();
}

void
DrawingContext::draw(Drawable* request)
{
  drawingrequests.push_back(request);
}

void
DrawingContext::draw(const Sprite& sprite, const Vector2f& pos, float z_pos)
{
  draw(sprite.get_current_surface(),
       SurfaceDrawingParameters()
       .set_pos(pos + sprite.get_offset() * sprite.get_scale())
       .set_blend_func(sprite.get_blend_sfactor(), sprite.get_blend_dfactor())
       .set_color(sprite.get_color())
       .set_scale(sprite.get_scale()),
       z_pos);
}

void
DrawingContext::draw(const Surface surface, const Vector2f& pos, const Quad& quad,
                     const DrawingParameters& params, float z_pos)
{
  draw(new SurfaceQuadDrawable(surface, pos, quad, params, z_pos,
                               modelview_stack.back()));
}

void
DrawingContext::draw(const Surface surface, const SurfaceDrawingParameters& params, float z_pos)
{
  draw(new SurfaceDrawable(surface, params, z_pos,
                           modelview_stack.back()));
}

void
DrawingContext::draw(const Surface surface, const Vector2f& pos, float z, float alpha)
{
  draw(surface, pos.x, pos.y, z, alpha);
}

void
DrawingContext::draw(Surface surface, float x, float y, float z, float )
{
  draw(new SurfaceDrawable(surface,
                                 SurfaceDrawingParameters().set_pos(Vector2f(x, y)),
                                 z, modelview_stack.back()));
}

void
DrawingContext::draw(const std::string& text, float x, float y, float z)
{ 
  draw(new TextDrawable(text, Vector2f(x, y), z, modelview_stack.back()));
}

void
DrawingContext::draw_control(const Surface surface, const Vector2f& pos, float angle, float z_pos)
{
  draw(new ControlDrawable(surface, pos, angle, z_pos, modelview_stack.back()));
}

void
DrawingContext::fill_screen(const Color& color)
{
  draw(new FillScreenDrawable(color));
}

void
DrawingContext::fill_pattern(const Texture& pattern, const Vector2f& offset)
{
  draw(new FillScreenPatternDrawable(pattern, offset));
}

void
DrawingContext::rotate(float angle, float x, float y, float z)
{
  float len2 = x*x+y*y+z*z;
  if (len2 != 1.0)
    {
      float len = sqrtf(len2);
      x /= len;
      y /= len;
      z /= len;
    }

  float c = cosf(angle*3.14159265f/180.0f);
  float s = sinf(angle*3.14159265f/180.0f);

  Matrix matrix = Matrix::identity();
  matrix[0]  = x*x*(1-c)+c;
  matrix[1]  = y*x*(1-c)+z*s;
  matrix[2]  = x*z*(1-c)-y*s;

  matrix[4]  = x*y*(1-c)-z*s;
  matrix[5]  = y*y*(1-c)+c;
  matrix[6]  = y*z*(1-c)+x*s;

  matrix[8]  = x*z*(1-c)+y*s;
  matrix[9]  = y*z*(1-c)-x*s;
  matrix[10] = z*z*(1-c)+c;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::scale(float x, float y, float z)
{
  Matrix matrix = Matrix::identity();
  matrix[0]  = x;
  matrix[5]  = y;
  matrix[10] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::translate(float x, float y, float z)
{
  Matrix matrix = Matrix::identity();
  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::mult(const Matrix& matrix)
{
  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::push_modelview()
{
  modelview_stack.push_back(modelview_stack.back());
}

void
DrawingContext::pop_modelview()
{
  modelview_stack.pop_back();
  assert(!modelview_stack.empty());
}

void
DrawingContext::set_modelview(const Matrix& matrix)
{
  modelview_stack.back() = matrix;
}

void
DrawingContext::reset_modelview()
{
  modelview_stack.clear();
  modelview_stack.push_back(Matrix::identity());
}

Rectf
DrawingContext::get_clip_rect()
{
  // FIXME: Need to check the modelview matrix
  return Rectf(Vector2f(modelview_stack.back()[12],
                        modelview_stack.back()[13]),
               Sizef(800, 600));
}

void
DrawingContext::draw_line(const Line& line, const Color& color, float z_pos)
{
  draw_line(line.p1, line.p2, color, z_pos);
}

void
DrawingContext::draw_line(const Vector2f& pos1, const Vector2f& pos2, const Color& color, float z_pos)
{
  VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_LINES);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(pos1.x, pos1.y);

  array->color(color);
  array->vertex(pos2.x, pos2.y);

  draw(array);
}

void
DrawingContext::draw_quad(const Quad& quad, const Color& color, float z_pos)
{
  VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_LINE_LOOP);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(quad.p1.x, quad.p1.y);

  array->color(color);
  array->vertex(quad.p2.x, quad.p2.y);

  array->color(color);
  array->vertex(quad.p3.x, quad.p3.y);

  array->color(color);
  array->vertex(quad.p4.x, quad.p4.y);
  
  draw(array);
}

void
DrawingContext::fill_quad(const Quad& quad, const Color& color, float z_pos)
{
  VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_QUADS);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(quad.p1.x, quad.p1.y);

  array->color(color);
  array->vertex(quad.p2.x, quad.p2.y);

  array->color(color);
  array->vertex(quad.p3.x, quad.p3.y);

  array->color(color);
  array->vertex(quad.p4.x, quad.p4.y);

  draw(array); 
}

void
DrawingContext::draw_rect(const Rectf& rect, const Color& color, float z_pos)
{
  VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_LINE_LOOP);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(rect.left, rect.top);

  array->color(color);
  array->vertex(rect.right, rect.top);  

  array->color(color);
  array->vertex(rect.right, rect.bottom);  

  array->color(color);
  array->vertex(rect.left, rect.bottom);  
  
  draw(array);
}

void
DrawingContext::fill_rect(const Rectf& rect, const Color& color, float z_pos)
{
  VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_QUADS);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(rect.left, rect.top);

  array->color(color);
  array->vertex(rect.right, rect.top);

  array->color(color);
  array->vertex(rect.right, rect.bottom);

  array->color(color);
  array->vertex(rect.left, rect.bottom);

  draw(array);
}

/* EOF */
