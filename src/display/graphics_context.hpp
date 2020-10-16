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

#ifndef HEADER_WINDSTILLE_DISPLAY_GRAPHICSCONTEXT_HPP
#define HEADER_WINDSTILLE_DISPLAY_GRAPHICSCONTEXT_HPP

#include <vector>

#include <glm/glm.hpp>

#include <geom/fwd.hpp>

#include "display/framebuffer.hpp"

class Color;

class GraphicsContext
{
public:
  GraphicsContext();
  ~GraphicsContext();

  void fill_quad(const geom::quad& quad, const Color& color);
  void draw_quad(const geom::quad& quad, const Color& color);

  void fill_rect(const geom::frect& rect, const Color& color);
  void draw_rect(const geom::frect& rect, const Color& color);

  void fill_rounded_rect(const geom::frect& rect, float radius, const Color& color);
  void draw_rounded_rect(const geom::frect& rect, float radius, const Color& color);

  /** Same as draw_line, but in addition draw a normal on top of the line */
  void draw_line_with_normal(const geom::line& line, const Color& color);

  void draw_line(const geom::line& line, const Color& color);
  void draw_line(const glm::vec2& pos1, const glm::vec2& pos2, const Color& color);

  void draw_circle(const glm::vec2& pos, float radius, const Color& color, int segments = 16);
  void fill_circle(const glm::vec2& pos, float radius, const Color& color, int segments = 16);

  void draw_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments = 16);
  void fill_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments = 16);

  void draw_grid(const glm::vec2& offset, const geom::fsize& size, const Color& color);

  void push_cliprect(const geom::irect& rect);
  void pop_cliprect();

  void push_framebuffer(FramebufferPtr framebuffer);
  void pop_framebuffer();
  FramebufferPtr get_framebuffer();

  void set_aspect_size(geom::isize const& aspect_size);
  geom::isize size() const;

  void ortho(float left, float right, float bottom, float top, float near, float far);
  void matrix_mode(int mode);
  void load_identity();
  void push_matrix();
  void mult_matrix(glm::mat4 const& mat);
  void translate(float x, float y, float z);
  void scale(float x, float y, float z);
  void rotate(float degree, float x, float y, float z);
  void pop_matrix();

private:
  geom::isize m_aspect_size;
  std::vector<geom::irect> m_cliprects;

private:
  GraphicsContext(const GraphicsContext&) = delete;
  GraphicsContext& operator=(const GraphicsContext&) = delete;
};

#endif

/* EOF */
