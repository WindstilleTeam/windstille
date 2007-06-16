/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_DISPLAY_DISPLAY_HPP
#define HEADER_DISPLAY_DISPLAY_HPP

#include <vector>
#include <SDL.h>
#include "math/rect.hpp"
#include "math/line.hpp"
#include "color.hpp"
#include "display/framebuffer.hpp"

class Display
{
private:
  static std::vector<Rect> cliprects;
  static SDL_Surface* window;

public:
  static void fill_rect(const Rectf& rect, const Color& color);
  static void draw_rect(const Rectf& rect, const Color& color);

  static void fill_rounded_rect(const Rectf& rect, float radius, const Color& color);
  static void draw_rounded_rect(const Rectf& rect, float radius, const Color& color);

  /** Same as draw_line, but in addition draw a normal on top of the line */
  static void draw_segment(const Line& line, const Color& color);

  static void draw_line(const Line& line, const Color& color);
  static void draw_line(const Vector& pos1, const Vector& pos2, const Color& color);

  static void draw_circle(const Vector& pos, float radius, const Color& color);
  static void fill_circle(const Vector& pos, float radius, const Color& color);

  static int  get_width();
  static int  get_height();

  static void set_fullscreen(bool fullscreen);

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
