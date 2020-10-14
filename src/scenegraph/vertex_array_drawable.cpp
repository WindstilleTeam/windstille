/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "scenegraph/vertex_array_drawable.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "display/opengl_state.hpp"

VertexArrayDrawable::VertexArrayDrawable(const glm::vec2& pos_, float z_pos_,
                                         const glm::mat4& modelview_) :
  Drawable(pos_, z_pos_, modelview_),
  mode(GL_QUADS),
  blend_sfactor(GL_SRC_ALPHA),
  blend_dfactor(GL_ONE_MINUS_SRC_ALPHA),
  texture(),
  colors(),
  texcoords(),
  vertices()
{
}

int
VertexArrayDrawable::num_vertices() const
{
  return static_cast<int>(vertices.size())/3;
}

void
VertexArrayDrawable::clear()
{
  colors.clear();
  texcoords.clear();
  vertices.clear();
}

void
VertexArrayDrawable::render(unsigned int mask)
{
  render(0, num_vertices());
}

void
VertexArrayDrawable::render(int start, int end)
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
  glMultMatrixf(glm::value_ptr(modelview));

  if (mode == GL_LINES ||
      mode == GL_LINE_LOOP)
  {
    // FIXME: Hack: make this configurable
    glLineWidth(2.0f);
    glDrawArrays(mode, start, end);
    glLineWidth(1.0f);
  }
  else
  {
    glDrawArrays(mode, start, end);
  }

  glPopMatrix();
}

void
VertexArrayDrawable::vertex(const glm::vec2& vec, float z)
{
  vertex(vec.x, vec.y, z);
}

void
VertexArrayDrawable::vertex(float x, float y, float z)
{
  vertices.push_back(x + pos.x);
  vertices.push_back(y + pos.y);
  vertices.push_back(z);
}

void
VertexArrayDrawable::texcoord(float u, float v)
{
  texcoords.push_back(u);
  texcoords.push_back(v);
}

void
VertexArrayDrawable::add_texcoords(const geom::frect& rect)
{
  texcoords.push_back(rect.left());
  texcoords.push_back(rect.top());
  texcoords.push_back(rect.right());
  texcoords.push_back(rect.top());
  texcoords.push_back(rect.right());
  texcoords.push_back(rect.bottom());
  texcoords.push_back(rect.left());
  texcoords.push_back(rect.bottom());
}

void
VertexArrayDrawable::add_texcoords(const float* coords, size_t n)
{
  assert(n % 2 == 0);
  for(size_t i = 0; i < n; ++i)
  {
    texcoords.push_back(coords[i]);
  }
}

void
VertexArrayDrawable::color(const Color& color_)
{
  colors.push_back(static_cast<unsigned char>(color_.r * 255));
  colors.push_back(static_cast<unsigned char>(color_.g * 255));
  colors.push_back(static_cast<unsigned char>(color_.b * 255));
  colors.push_back(static_cast<unsigned char>(color_.a * 255));
}

void
VertexArrayDrawable::set_texture(TexturePtr texture_)
{
  texture = texture_;
}

void
VertexArrayDrawable::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

void
VertexArrayDrawable::set_mode(GLenum mode_)
{
  mode = mode_;
}

/* EOF */
