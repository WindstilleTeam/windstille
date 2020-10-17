//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "display/gl_vertex_arrays.hpp"

#include <iostream>

#include "display/assert_gl.hpp"
#include "display/graphics_context.hpp"

GLVertexArrays::GLVertexArrays(GraphicsContext& gc) :
  m_gc(gc),
  m_vao(),
  m_positions_buffer(),
  m_texcoords_buffer(),
  m_color_buffer()
{
  assert_gl();

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_positions_buffer);
  glGenBuffers(1, &m_texcoords_buffer);
  glGenBuffers(1, &m_color_buffer);

  assert_gl();
}

GLVertexArrays::~GLVertexArrays()
{
  glDeleteBuffers(1, &m_positions_buffer);
  glDeleteBuffers(1, &m_texcoords_buffer);
  glDeleteBuffers(1, &m_color_buffer);
  glDeleteVertexArrays(1, &m_vao);
}

void
GLVertexArrays::bind()
{
  assert_gl();

  glBindVertexArray(m_vao);

  assert_gl();
}

void
GLVertexArrays::set_positions(std::span<float const> data)
{
  assert_gl();

  assert(data.size() % 3 == 0);

  glBindBuffer(GL_ARRAY_BUFFER, m_positions_buffer);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

  assert_gl();

  int loc = m_gc.get_default_shader()->get_attrib_location("position");
  if (loc == -1) {
    std::cout << "failed to set position\n";
  } else {
    assert_gl();
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    assert_gl();
    glEnableVertexAttribArray(loc);
    assert_gl();
  }

  assert_gl();
}

void
GLVertexArrays::set_texcoords(std::span<float const> data)
{
  assert_gl();

  glBindBuffer(GL_ARRAY_BUFFER, m_texcoords_buffer);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

  int loc = m_gc.get_default_shader()->get_attrib_location("texcoord");
  if (loc != -1) {
    assert_gl();
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    assert_gl();
    glEnableVertexAttribArray(loc);
  } else {
    std::cout << "failed to set texcoord\n";
  }

  assert_gl();
}

void
GLVertexArrays::set_colors(std::span<float const> data)
{
  assert_gl();

  glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

  int loc = m_gc.get_default_shader()->get_attrib_location("diffuse");
  if (loc != -1) {
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(loc);
  } else {
    std::cout << "failed to set colors\n";
  }

  assert_gl();
}

/* EOF */
