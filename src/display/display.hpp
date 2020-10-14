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

#ifndef HEADER_WINDSTILLE_DISPLAY_DISPLAY_HPP
#define HEADER_WINDSTILLE_DISPLAY_DISPLAY_HPP

#include <vector>

#include <glm/glm.hpp>

#include <geom/rect.hpp>
#include <geom/size.hpp>

#include "display/framebuffer.hpp"

class Color;
class Quad;
class Line;

class Display
{
private:
  static std::vector<geom::irect> cliprects;

public:
  static geom::isize aspect_size;

public:
  static void fill_quad(const Quad& quad, const Color& color);
  static void draw_quad(const Quad& quad, const Color& color);

  static void fill_rect(const geom::frect& rect, const Color& color);
  static void draw_rect(const geom::frect& rect, const Color& color);

  static void fill_rounded_rect(const geom::frect& rect, float radius, const Color& color);
  static void draw_rounded_rect(const geom::frect& rect, float radius, const Color& color);

  /** Same as draw_line, but in addition draw a normal on top of the line */
  static void draw_line_with_normal(const Line& line, const Color& color);

  static void draw_line(const Line& line, const Color& color);
  static void draw_line(const glm::vec2& pos1, const glm::vec2& pos2, const Color& color);

  static void draw_circle(const glm::vec2& pos, float radius, const Color& color, int segments = 16);
  static void fill_circle(const glm::vec2& pos, float radius, const Color& color, int segments = 16);

  static void draw_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments = 16);
  static void fill_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments = 16);

  static void draw_grid(const glm::vec2& offset, const geom::fsize& size, const Color& color);

  static int  get_width();
  static int  get_height();

  static geom::isize get_size() { return geom::isize(get_width(), get_height()); }

  static void push_cliprect(const geom::irect& rect);
  static void pop_cliprect();

  static void push_framebuffer(FramebufferPtr framebuffer);
  static void pop_framebuffer();
  static FramebufferPtr get_framebuffer();

  static void save_screenshot(std::filesystem::path const& filename);
};

#endif

/* EOF */
