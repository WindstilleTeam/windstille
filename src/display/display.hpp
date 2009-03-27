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

#ifndef HEADER_WINDSTILLE_DISPLAY_DISPLAY_HPP
#define HEADER_WINDSTILLE_DISPLAY_DISPLAY_HPP

#include <vector>
#include <SDL.h>
#include "math/size.hpp"
#include "math/rect.hpp"
#include "math/line.hpp"
#include "color.hpp"
#include "display/framebuffer.hpp"

class Display
{
private:
  static std::vector<Rect> cliprects;

public:
  static Size aspect_size;
  
public:
  static void fill_rect(const Rectf& rect, const Color& color);
  static void draw_rect(const Rectf& rect, const Color& color);

  static void fill_rounded_rect(const Rectf& rect, float radius, const Color& color);
  static void draw_rounded_rect(const Rectf& rect, float radius, const Color& color);

  /** Same as draw_line, but in addition draw a normal on top of the line */
  static void draw_line_with_normal(const Line& line, const Color& color);

  static void draw_line(const Line& line, const Color& color);
  static void draw_line(const Vector2f& pos1, const Vector2f& pos2, const Color& color);

  static void draw_circle(const Vector2f& pos, float radius, const Color& color, int segments = 16);
  static void fill_circle(const Vector2f& pos, float radius, const Color& color, int segments = 16);

  static void draw_arc(const Vector2f& pos, float radius, float start, float end, const Color& color, int segments = 16);
  static void fill_arc(const Vector2f& pos, float radius, float start, float end, const Color& color, int segments = 16);

  static void draw_grid(const Vector2f& offset, const Sizef& size, const Color& color);

  static int  get_width();
  static int  get_height();

  static void push_cliprect(const Rect& rect);
  static void pop_cliprect();

  static void push_framebuffer(Framebuffer& framebuffer);
  static void pop_framebuffer();
  static Framebuffer get_framebuffer();

  static void set_gamma(float r, float g, float b);

  static void save_screenshot(const std::string& filename);

  static void init();
};

#endif

/* EOF */
