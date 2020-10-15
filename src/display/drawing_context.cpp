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

#include "display/drawing_context.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <geom/line.hpp>

#include "display/compositor.hpp"
#include "display/graphics_context.hpp"
#include "display/drawing_parameters.hpp"
#include "display/opengl_state.hpp"
#include "display/scene_context.hpp"
#include "display/surface_drawing_parameters.hpp"

#include "scenegraph/control_drawable.hpp"
#include "scenegraph/fill_screen_drawable.hpp"
#include "scenegraph/fill_screen_pattern_drawable.hpp"
#include "scenegraph/surface_drawable.hpp"
#include "scenegraph/surface_quad_drawable.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

struct DrawablesSorter
{
  bool operator()(std::unique_ptr<Drawable> const& a, std::unique_ptr<Drawable> const& b) {
    return a->get_z_pos() < b->get_z_pos();
  }
};

DrawingContext::DrawingContext() :
  drawingrequests(),
  modelview_stack()
{
  modelview_stack.push_back(glm::mat4(1.0f));
}

DrawingContext::~DrawingContext()
{
  clear();
}

void
DrawingContext::render(GraphicsContext& gc)
{
  std::stable_sort(drawingrequests.begin(), drawingrequests.end(), DrawablesSorter());

  for(Drawables::iterator i = drawingrequests.begin(); i != drawingrequests.end(); ++i)
  {
    (*i)->render(gc, ~0u);
  }
}

void
DrawingContext::clear()
{
  drawingrequests.clear();
}

void
DrawingContext::draw(std::unique_ptr<Drawable> request)
{
  drawingrequests.push_back(std::move(request));
}

void
DrawingContext::draw(SurfacePtr surface, const glm::vec2& pos, const geom::quad& quad,
                     const DrawingParameters& params, float z_pos)
{
  draw(std::make_unique<SurfaceQuadDrawable>(surface, pos, quad, params, z_pos,
                                             modelview_stack.back()));
}

void
DrawingContext::draw(SurfacePtr surface, const SurfaceDrawingParameters& params, float z_pos)
{
  draw(std::make_unique<SurfaceDrawable>(surface, params, z_pos,
                                         modelview_stack.back()));
}

void
DrawingContext::draw(SurfacePtr surface, const glm::vec2& pos, float z, float alpha)
{
  draw(surface, pos.x, pos.y, z, alpha);
}

void
DrawingContext::draw(SurfacePtr surface, float x, float y, float z, float )
{
  draw(std::make_unique<SurfaceDrawable>(surface,
                                         SurfaceDrawingParameters().set_pos(glm::vec2(x, y)),
                                         z, modelview_stack.back()));
}

void
DrawingContext::draw_control(SurfacePtr surface, const glm::vec2& pos, float angle, float z_pos)
{
  draw(std::make_unique<ControlDrawable>(surface, pos, angle, z_pos, modelview_stack.back()));
}

void
DrawingContext::fill_screen(const Color& color)
{
  draw(std::make_unique<FillScreenDrawable>(color));
}

void
DrawingContext::fill_pattern(TexturePtr pattern, const glm::vec2& offset)
{
  draw(std::make_unique<FillScreenPatternDrawable>(pattern, offset));
}

void
DrawingContext::rotate(float angle, float x, float y, float z)
{
  float len2 = x*x+y*y+z*z;
  if (len2 != 1.0f)
  {
    float len = sqrtf(len2);
    x /= len;
    y /= len;
    z /= len;
  }

  float c = cosf(angle*3.14159265f/180.0f);
  float s = sinf(angle*3.14159265f/180.0f);

  glm::mat4 matrix(1.0f);
  float* mp = glm::value_ptr(matrix);
  mp[0]  = x*x*(1-c)+c;
  mp[1]  = y*x*(1-c)+z*s;
  mp[2]  = x*z*(1-c)-y*s;

  mp[4]  = x*y*(1-c)-z*s;
  mp[5]  = y*y*(1-c)+c;
  mp[6]  = y*z*(1-c)+x*s;

  mp[8]  = x*z*(1-c)+y*s;
  mp[9]  = y*z*(1-c)-x*s;
  mp[10] = z*z*(1-c)+c;

  modelview_stack.back() = modelview_stack.back() * matrix;
}

void
DrawingContext::scale(float x, float y, float z)
{
  glm::mat4 matrix(1.0f);
  glm::value_ptr(matrix)[0]  = x;
  glm::value_ptr(matrix)[5]  = y;
  glm::value_ptr(matrix)[10] = z;
  modelview_stack.back() = modelview_stack.back() * matrix;
}

void
DrawingContext::translate(float x, float y, float z)
{
  glm::mat4 matrix = glm::mat4(1.0f);
  glm::value_ptr(matrix)[12] = x;
  glm::value_ptr(matrix)[13] = y;
  glm::value_ptr(matrix)[14] = z;
  modelview_stack.back() = modelview_stack.back() * matrix;
}

void
DrawingContext::mult(const glm::mat4& matrix)
{
  modelview_stack.back() = modelview_stack.back() * matrix;
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
DrawingContext::set_modelview(const glm::mat4& matrix)
{
  modelview_stack.back() = matrix;
}

void
DrawingContext::reset_modelview()
{
  modelview_stack.clear();
  modelview_stack.push_back(glm::mat4(1.0f));
}

geom::frect
DrawingContext::get_clip_rect()
{
  // FIXME: Need to check the modelview matrix
  return geom::frect(glm::vec2(glm::value_ptr(modelview_stack.back())[12],
                         glm::value_ptr(modelview_stack.back())[13]),
               geom::fsize(800, 600));
}

void
DrawingContext::draw_line(const geom::line& line, const Color& color, float z_pos)
{
  draw_line(line.p1, line.p2, color, z_pos);
}

void
DrawingContext::draw_line(const glm::vec2& pos1, const glm::vec2& pos2, const Color& color, float z_pos)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_LINES);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(pos1.x, pos1.y);

  array->color(color);
  array->vertex(pos2.x, pos2.y);

  draw(std::move(array));
}

void
DrawingContext::draw_quad(const geom::quad& quad, const Color& color, float z_pos)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), z_pos, modelview_stack.back());

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

  draw(std::move(array));
}

void
DrawingContext::fill_quad(const geom::quad& quad, const Color& color, float z_pos)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), z_pos, modelview_stack.back());

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

  draw(std::move(array));
}

void
DrawingContext::draw_rect(const geom::frect& rect, const Color& color, float z_pos)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_LINE_LOOP);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(rect.left(), rect.top());

  array->color(color);
  array->vertex(rect.right(), rect.top());

  array->color(color);
  array->vertex(rect.right(), rect.bottom());

  array->color(color);
  array->vertex(rect.left(), rect.bottom());

  draw(std::move(array));
}

void
DrawingContext::fill_rect(const geom::frect& rect, const Color& color, float z_pos)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), z_pos, modelview_stack.back());

  array->set_mode(GL_QUADS);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  array->color(color);
  array->vertex(rect.left(), rect.top());

  array->color(color);
  array->vertex(rect.right(), rect.top());

  array->color(color);
  array->vertex(rect.right(), rect.bottom());

  array->color(color);
  array->vertex(rect.left(), rect.bottom());

  draw(std::move(array));
}

/* EOF */
