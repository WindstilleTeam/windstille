/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>

#include "display/opengl_state.hpp"
#include "vertex_array_drawing_request.hpp"

VertexArrayDrawingRequest::VertexArrayDrawingRequest(const Vector2f& pos_, float z_pos_, 
                                                     const Matrix& modelview_)
  : DrawingRequest(pos_, z_pos_, modelview_)
{
  blend_sfactor = GL_SRC_ALPHA;
  blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
  mode = GL_QUADS;
}

int
VertexArrayDrawingRequest::num_vertices() const
{
  return vertices.size()/3;
}

void
VertexArrayDrawingRequest::draw(const Texture& /*tmp_texture*/)
{
  draw(0, num_vertices());
}

void
VertexArrayDrawingRequest::draw(int start, int end)
{
  assert(!vertices.empty());
  assert(texcoords.empty() || int(texcoords.size()/2) == num_vertices());
  assert(colors.empty() || int(colors.size()/4) == num_vertices());

  OpenGLState state;

  glClear(GL_DEPTH_BUFFER_BIT);
  state.disable(GL_DEPTH_TEST);
  state.enable(GL_BLEND);
  state.set_blend_func(blend_sfactor, blend_dfactor);
  
  if (texture)
    {
      state.bind_texture(texture);
    }

  if (!colors.empty())
    {
      state.enable_client_state(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, &*colors.begin());
    }
  else
    {
      state.disable_client_state(GL_COLOR_ARRAY);
      state.color(Color(1.0f, 1.0f, 1.0f));
    }

  if (!texcoords.empty())
    {
      state.enable_client_state(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, &*texcoords.begin());
    }
  else
    {
      state.disable_client_state(GL_TEXTURE_COORD_ARRAY);
    }

  // FIXME: Might be worth to not use VertexArrays when we have a pretty small number of vertices
  state.disable_client_state(GL_NORMAL_ARRAY);
  state.enable_client_state(GL_VERTEX_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, &*vertices.begin());

  state.activate();

  glPushMatrix();
  glMultMatrixf(modelview.matrix);

  glDrawArrays(mode, start, end);

  glPopMatrix();
}

void
VertexArrayDrawingRequest::vertex(const Vector2f& vec, float z)
{
  vertex(vec.x, vec.y, z);
}

void
VertexArrayDrawingRequest::vertex(float x, float y, float z)
{
  vertices.push_back(x + pos.x);
  vertices.push_back(y + pos.y);
  vertices.push_back(z);
}

void
VertexArrayDrawingRequest::texcoord(float u, float v)
{
  texcoords.push_back(u);
  texcoords.push_back(v);
}

void
VertexArrayDrawingRequest::add_texcoords(const Rectf& rect)
{
  texcoords.push_back(rect.left);
  texcoords.push_back(rect.top);
  texcoords.push_back(rect.right);
  texcoords.push_back(rect.top);
  texcoords.push_back(rect.right);
  texcoords.push_back(rect.bottom);
  texcoords.push_back(rect.left);
  texcoords.push_back(rect.bottom);
}

void
VertexArrayDrawingRequest::add_texcoords(const float* coords, size_t n)
{
  assert(n % 2 == 0);
  for(size_t i = 0; i < n; ++i)
  {
    texcoords.push_back(coords[i]);
  }
}

void
VertexArrayDrawingRequest::color(const Color& color)
{
  colors.push_back(static_cast<unsigned char>(color.r * 255));
  colors.push_back(static_cast<unsigned char>(color.g * 255));
  colors.push_back(static_cast<unsigned char>(color.b * 255));
  colors.push_back(static_cast<unsigned char>(color.a * 255));
}

void
VertexArrayDrawingRequest::set_texture(Texture texture_)
{
  texture = texture_;
}

void
VertexArrayDrawingRequest::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

void
VertexArrayDrawingRequest::set_mode(GLenum mode_)
{
  mode = mode_;
}

/* EOF */
