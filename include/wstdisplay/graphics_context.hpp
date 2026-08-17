// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_DISPLAY_GRAPHICSCONTEXT_HPP
#define HEADER_WINDSTILLE_DISPLAY_GRAPHICSCONTEXT_HPP

#include <stack>
#include <vector>

#include <glm/glm.hpp>

#include <geom/fwd.hpp>
#include <surf/fwd.hpp>

#include "framebuffer.hpp"
#include "gl_vertex_arrays.hpp"
#include "shader_program.hpp"

namespace wstdisplay {

class GLVertexArrays;

class GraphicsContext
{
public:
  GraphicsContext();
  ~GraphicsContext();

  void clear(surf::Color const& color);

  void fill_quad(const geom::fquad& quad, const surf::Color& color);
  void draw_quad(const geom::fquad& quad, const surf::Color& color);

  void fill_rect(const geom::frect& rect, const surf::Color& color);
  void draw_rect(const geom::frect& rect, const surf::Color& color);

  void fill_rounded_rect(const geom::frect& rect, float radius, const surf::Color& color);
  void draw_rounded_rect(const geom::frect& rect, float radius, const surf::Color& color);

  /** Same as draw_line, but in addition draw a normal on top of the line */
  void draw_line_with_normal(const geom::fline& line, const surf::Color& color);

  void draw_line(const geom::fline& line, const surf::Color& color);
  void draw_line(const geom::fpoint& pos1, const geom::fpoint& pos2, const surf::Color& color);

  void draw_circle(const geom::fpoint& pos, float radius, const surf::Color& color, int segments = 16);
  void fill_circle(const geom::fpoint& pos, float radius, const surf::Color& color, int segments = 16);

  void draw_arc(const geom::fpoint& pos, float radius, float start, float end, const surf::Color& color, int segments = 16);
  void fill_arc(const geom::fpoint& pos, float radius, float start, float end, const surf::Color& color, int segments = 16);

  void draw_grid(const geom::fpoint& offset, const geom::fsize& size, const surf::Color& color);

  void push_cliprect(const geom::irect& rect);
  void pop_cliprect();

  void push_framebuffer(FramebufferPtr framebuffer);
  void pop_framebuffer();
  FramebufferPtr get_framebuffer();

  /** Only valid with ortho projection */
  geom::isize size() const;

  void set_ortho(geom::isize const& size);
  void set_projection(glm::mat4 const& mat);
  glm::mat4 const& get_projection() const { return m_projection; }

  void set_modelview(glm::mat4 const& mat);
  glm::mat4 const& get_modelview() const { return m_modelview_stack.top(); }

  void push_matrix();
  void pop_matrix();
  void mult_matrix(glm::mat4 const& mat);
  void translate(float x, float y, float z);
  void scale(float x, float y, float z);
  void rotate(float degree, float x, float y, float z);

  ShaderProgramPtr get_default_shader() const { return m_default_shader; }
  GLVertexArrays& get_va() { return m_vertex_arrays; }
  TexturePtr get_white_texture() const { return m_white_texture; }

private:
  geom::isize m_size;
  std::vector<geom::irect> m_cliprects;

  ShaderProgramPtr m_default_shader;
  TexturePtr m_white_texture;
  std::stack<glm::mat4> m_modelview_stack;
  glm::mat4 m_projection;
  GLVertexArrays m_vertex_arrays;

private:
  GraphicsContext(const GraphicsContext&) = delete;
  GraphicsContext& operator=(const GraphicsContext&) = delete;
};

} // namespace wstdisplay

#endif

/* EOF */
