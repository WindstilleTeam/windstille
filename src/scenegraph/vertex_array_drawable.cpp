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

#include "display/graphics_context.hpp"
#include "display/opengl_state.hpp"

VertexArrayDrawable::VertexArrayDrawable() :
  VertexArrayDrawable({0.0f, 0.0f}, 0.0f, glm::mat4(1.0f))
{}

VertexArrayDrawable::VertexArrayDrawable(glm::vec2 const& pos_, float z_pos_,
                                         glm::mat4 const& modelview_) :
  Drawable(pos_, z_pos_, modelview_),
  m_mode(GL_QUADS),
  m_blend_sfactor(GL_SRC_ALPHA),
  m_blend_dfactor(GL_ONE_MINUS_SRC_ALPHA),
  m_depth_test(false),
  m_textures(),
  m_colors(),
  m_texcoords(),
  m_normals(),
  m_vertices(),
  m_indices()
{
}

int
VertexArrayDrawable::num_vertices() const
{
  return static_cast<int>(m_vertices.size()) / 3;
}

void
VertexArrayDrawable::clear()
{
  m_textures.clear();
  m_colors.clear();
  m_texcoords.clear();
  m_normals.clear();
  m_vertices.clear();
  m_indices.clear();
}

void
VertexArrayDrawable::render(GraphicsContext& gc, unsigned int mask)
{
  assert(!m_vertices.empty());

  assert(m_texcoords.empty() || int(m_texcoords.size() / 2) == num_vertices());
  assert(m_normals.empty() || int(m_normals.size() / 3) == num_vertices());
  assert(m_colors.empty() || int(m_colors.size() / 4) == num_vertices());

  OpenGLState state;

  if (m_depth_test) {
    state.enable(GL_DEPTH_TEST);
  } else {
    state.disable(GL_DEPTH_TEST);
  }

  state.enable(GL_BLEND);
  state.set_blend_func(m_blend_sfactor, m_blend_dfactor);

  for (auto const& it : m_textures) {
    state.bind_texture(it.second, it.first);
  }

  if (!m_colors.empty()) {
    state.enable_client_state(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, m_colors.data());
  } else {
    state.disable_client_state(GL_COLOR_ARRAY);
    state.color(Color(1.0f, 1.0f, 1.0f));
  }

  if (!m_texcoords.empty()) {
    state.enable_client_state(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, m_texcoords.data());
  } else {
    state.disable_client_state(GL_TEXTURE_COORD_ARRAY);
  }

  if (!m_normals.empty()) {
    state.enable_client_state(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, m_normals.data());
  } else {
    state.disable_client_state(GL_NORMAL_ARRAY);
  }

  state.enable_client_state(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_vertices.data());

  state.activate();

  gc.push_matrix();
  gc.mult_matrix(modelview);

  // FIXME: Hack: make this configurable
  if (m_mode == GL_LINES || m_mode == GL_LINE_LOOP) {
    glLineWidth(2.0f);
  }

  if (m_indices.empty()) {
    glDrawArrays(m_mode, 0, num_vertices());
  } else {
    glDrawElements(m_mode, static_cast<GLsizei>(m_indices.size()),
                   GL_UNSIGNED_SHORT, m_indices.data());
  }

  if (m_mode == GL_LINES || m_mode == GL_LINE_LOOP) {
    glLineWidth(1.0f);
  }

  gc.pop_matrix();
}

void
VertexArrayDrawable::vertex(glm::vec2 const& vec, float z)
{
  vertex(vec.x, vec.y, z);
}

void
VertexArrayDrawable::vertex(int x, int y, int z)
{
  vertex(static_cast<float>(x),
         static_cast<float>(y),
         static_cast<float>(z));
}

void
VertexArrayDrawable::vertex(float x, float y, float z)
{
  m_vertices.push_back(x + pos.x);
  m_vertices.push_back(y + pos.y);
  m_vertices.push_back(z);
}

void
VertexArrayDrawable::texcoord(float u, float v)
{
  m_texcoords.push_back(u);
  m_texcoords.push_back(v);
}

void
VertexArrayDrawable::add_texcoords(geom::frect const& rect)
{
  m_texcoords.push_back(rect.left());
  m_texcoords.push_back(rect.top());
  m_texcoords.push_back(rect.right());
  m_texcoords.push_back(rect.top());
  m_texcoords.push_back(rect.right());
  m_texcoords.push_back(rect.bottom());
  m_texcoords.push_back(rect.left());
  m_texcoords.push_back(rect.bottom());
}

void
VertexArrayDrawable::add_texcoords(std::span<float const> data)
{
  assert(data.size() % 2 == 0);
  m_texcoords.insert(m_texcoords.end(), data.begin(), data.end());
}

void
VertexArrayDrawable::add_normals(std::span<float const> data)
{
  assert(data.size() % 3 == 0);
  m_normals.insert(m_normals.end(), data.begin(), data.end());
}

void
VertexArrayDrawable::add_indices(std::span<unsigned short int const> data)
{
  assert(data.size() % 3 == 0);
  m_indices.insert(m_indices.end(), data.begin(), data.end());
}

void
VertexArrayDrawable::add_vertices(std::span<float const> data)
{
  assert(data.size() % 3 == 0);
  m_vertices.insert(m_vertices.end(), data.begin(), data.end());
}

void
VertexArrayDrawable::normal(float x, float y, float z)
{
  m_normals.push_back(x);
  m_normals.push_back(y);
  m_normals.push_back(z);
}

void
VertexArrayDrawable::color(Color const& color_)
{
  m_colors.push_back(color_.r);
  m_colors.push_back(color_.g);
  m_colors.push_back(color_.b);
  m_colors.push_back(color_.a);
}

void
VertexArrayDrawable::set_texture(TexturePtr texture)
{
  m_textures[0] = texture;
}

void
VertexArrayDrawable::set_texture(int unit, TexturePtr texture)
{
  m_textures[unit] = texture;
}

void
VertexArrayDrawable::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  m_blend_sfactor = sfactor;
  m_blend_dfactor = dfactor;
}

void
VertexArrayDrawable::set_depth_test(bool depth_test)
{
  m_depth_test = depth_test;
}

void
VertexArrayDrawable::set_mode(GLenum mode)
{
  m_mode = mode;
}

/* EOF */
