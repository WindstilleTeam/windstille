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

#include "display/display.hpp"

#include <iostream>
#include <errno.h>
#include <fstream>
#include <jpeglib.h>
#include <png.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <numbers>

#include <geom/rect.hpp>
#include <geom/line.hpp>
#include <geom/quad.hpp>

#include "display/color.hpp"
#include "display/opengl_state.hpp"
#include "display/assert_gl.hpp"
#include "math/math.hpp"

geom::isize              Display::aspect_size;
std::vector<geom::irect> Display::cliprects;

namespace {
std::vector<FramebufferPtr> framebuffers;
} // namespace

void
Display::draw_line(const geom::line& line, const Color& color)
{
  draw_line(line.p1, line.p2, color);
}

void
Display::draw_line_with_normal(const geom::line& line, const Color& color)
{
  glm::vec2 normal = (line.p2 - line.p1);

  normal = glm::vec2(-normal.y, normal.x);
  normal = glm::normalize(normal);
  normal *= -32.0f;

  glm::vec2 p3 = line.p1 + 0.5f * (line.p2 - line.p1);

  draw_line(line,   color);
  draw_line(p3, p3 + normal, Color(0.0f, 1.0f, 1.0f));
}

void
Display::draw_line(const glm::vec2& pos1, const glm::vec2& pos2, const Color& color)
{
  OpenGLState state;

  state.enable(GL_LINE_SMOOTH);
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINES);
  glVertex2f(pos1.x, pos1.y);
  glVertex2f(pos2.x, pos2.y);
  glEnd();
}

void
Display::fill_quad(const geom::quad& quad, const Color& color)
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_QUADS);
  glVertex2f(quad.p1.x, quad.p1.y);
  glVertex2f(quad.p2.x, quad.p2.y);
  glVertex2f(quad.p3.x, quad.p3.y);
  glVertex2f(quad.p4.x, quad.p4.y);
  glEnd();
}

void
Display::draw_quad(const geom::quad& quad, const Color& color)
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINE_LOOP);
  glVertex2f(quad.p1.x, quad.p1.y);
  glVertex2f(quad.p2.x, quad.p2.y);
  glVertex2f(quad.p3.x, quad.p3.y);
  glVertex2f(quad.p4.x, quad.p4.y);
  glEnd();
}

void
Display::fill_rect(const geom::frect& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_QUADS);
  glVertex2f(rect.left(),  rect.top());
  glVertex2f(rect.right(), rect.top());
  glVertex2f(rect.right(), rect.bottom());
  glVertex2f(rect.left(),  rect.bottom());
  glEnd();
}

void
Display::draw_rect(const geom::frect& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINE_LOOP);
  glVertex2f(rect.left(),  rect.top());
  glVertex2f(rect.right(), rect.top());
  glVertex2f(rect.right(), rect.bottom());
  glVertex2f(rect.left(),  rect.bottom());
  glEnd();
}

void
Display::fill_rounded_rect(const geom::frect& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.width()/2, rect.height()/2));

  // inner rectangle
  geom::frect irect(rect.left()    + radius,
              rect.top()     + radius,
              rect.right()   - radius,
              rect.bottom()  - radius);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 8;
  glBegin(GL_QUAD_STRIP);
  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2.0f) / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2.0f) / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.top() - y);
    glVertex2f(irect.right() + x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2.0f) / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2.0f) / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.bottom() + y);
    glVertex2f(irect.right() + x, irect.bottom() + y);
  }
  glEnd();
}

void
Display::draw_rounded_rect(const geom::frect& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.width()/2, rect.height()/2));

  // inner rectangle
  geom::frect irect(rect.left()    + radius,
              rect.top()     + radius,
              rect.right()   - radius,
              rect.bottom()  - radius);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.enable(GL_LINE_SMOOTH);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 4;
  glBegin(GL_LINE_STRIP);
  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;

    glVertex2f(irect.right() + x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / static_cast<float>(n)) * radius;

    glVertex2f(irect.right() + x, irect.top() - y);
  }
  // go back to start
  glVertex2f(irect.left(), irect.top() - radius);

  glEnd();
}

int
Display::get_width()
{
  return Display::aspect_size.width();
}

int
Display::get_height()
{
  return Display::aspect_size.height();
}

void
Display::draw_circle(const glm::vec2& pos, float radius, const Color& color, int segments)
{
  assert(segments >= 0);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.enable(GL_LINE_SMOOTH);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  float n = static_cast<float>(segments) / 4.0f;
  glBegin(GL_LINE_STRIP);
  glVertex2f(radius + pos.x, pos.y);
  for(int i = 1; i < segments; ++i)
  {
    float x = cosf(static_cast<float>(i) * (std::numbers::pi_v<float> / 2.0f) / n) * radius;
    float y = sinf(static_cast<float>(i) * (std::numbers::pi_v<float> / 2.0f) / n) * radius;

    glVertex2f(x + pos.x, y + pos.y);
  }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
Display::fill_circle(const glm::vec2& pos, float radius, const Color& color, int segments)
{
  assert(segments >= 0);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  float n = static_cast<float>(segments) / 4.0f;
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(pos.x, pos.y);
  glVertex2f(radius + pos.x, pos.y);
  for(int i = 1; i < segments; ++i)
  {
    float x = cosf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / n) * radius;
    float y = sinf(static_cast<float>(i) * (std::numbers::pi_v<float>/2) / n) * radius;

    glVertex2f(x + pos.x, y + pos.y);
  }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
Display::draw_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    draw_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = (2.0f * std::numbers::pi_v<float>) / static_cast<float>(segments);

    if (start > end)
      std::swap(start, end);

    OpenGLState state;

    state.enable(GL_BLEND);
    state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    state.color(color);
    state.activate();

    start = glm::radians(start);
    end   = glm::radians(end);

    glBegin(GL_LINE_STRIP);
    glVertex2f(pos.x, pos.y);

    for(float angle = start; angle < end; angle += step)
      glVertex2f((cosf(angle) * radius) + pos.x,
                 (sinf(angle) * radius) + pos.y);

    glVertex2f((cosf(end) * radius) + pos.x,
               (sinf(end) * radius) + pos.y);
    glVertex2f(pos.x, pos.y);
    glEnd();
  }
}

void
Display::fill_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    fill_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = (2.0f * std::numbers::pi_v<float>) / static_cast<float>(segments);

    if (start > end)
      std::swap(start, end);

    OpenGLState state;

    state.enable(GL_BLEND);
    state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    state.color(color);
    state.activate();

    start = glm::radians(start);
    end   = glm::radians(end);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(pos.x, pos.y);

    for(float angle = start; angle < end; angle += step)
      glVertex2f((cosf(angle) * radius) + pos.x,
                 (sinf(angle) * radius) + pos.y);

    glVertex2f(cosf(end) * radius + pos.x,
               sinf(end) * radius + pos.y);

    glEnd();
  }
}

void
Display::draw_grid(const glm::vec2& offset, const geom::fsize& size, const Color& rgba)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(rgba);
  state.activate();

  glBegin(GL_LINES);
  //glColor4ub(rgba.r, rgba.g, rgba.b, rgba.a);

  float start_x = fmodf(offset.x, size.width());
  float start_y = fmodf(offset.y, size.height());

  for(float x = start_x; x < static_cast<float>(Display::get_width()); x += size.width())
  {
    glVertex2f(x, 0);
    glVertex2f(x, static_cast<float>(Display::get_height()));
  }

  for(float y = start_y; y < static_cast<float>(Display::get_height()); y += size.height())
  {
    glVertex2f(0, y);
    glVertex2f(static_cast<float>(Display::get_width()), y);
  }

  glEnd();
}

void
Display::push_cliprect(const geom::irect& rect_)
{
  geom::irect rect = rect_;

  if (!cliprects.empty())
  {
    rect = geom::irect(std::max(rect.left(), cliprects.back().left()),
                std::max(rect.top(),  cliprects.back().top()),
                std::min(rect.right(),  cliprects.back().right()),
                std::min(rect.bottom(), cliprects.back().bottom()));
  }

  cliprects.push_back(rect);

  glScissor(rect.left(), get_height() - rect.top() - rect.height(),
            rect.width(), rect.height());
  glEnable(GL_SCISSOR_TEST);
}

void
Display::pop_cliprect()
{
  assert(!cliprects.empty());

  cliprects.pop_back();

  if (!cliprects.empty())
  {
    const geom::irect& rect = cliprects.back();

    glScissor(rect.left(), get_height() - rect.top() - rect.height(),
              rect.width(), rect.height());
  }
  else
  {
    glDisable(GL_SCISSOR_TEST);
  }
}

void
Display::save_screenshot(std::filesystem::path const& filename)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  geom::isize size(viewport[2] / 4 * 4, viewport[3]);

  int len = size.width() * size.height() * 3;

  std::vector<GLbyte> pixels(len);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, size.width(), size.height(), GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  assert_gl("Display::save_screenshot()");

  if ((false))
  { // PPM saving
    int pitch = size.width() * 3;

    // save to ppm
    std::ofstream out(filename);
    out << "P6\n"
        << "# Windstille Screenshot\n"
        << size.width() << " " << size.height() << "\n"
        << "255\n";

    for(int y = size.height() - 1; y >= 0; --y)
      out.write(reinterpret_cast<const char*>(pixels.data() + y*pitch), pitch);

    out.close();
  }
  else if ((true))
  {
    FILE* m_out = fopen(filename.c_str(), "wb");

    if (!m_out)
    {
      std::ostringstream out;
      out << "FileJPEGCompressor(): Error: " << filename << ": " << strerror(errno);
      throw std::runtime_error(out.str());
    }
    else
    {
      int pitch = size.width() * 3;
      struct jpeg_compress_struct m_cinfo;
      struct jpeg_error_mgr m_jerr;

      jpeg_std_error(&m_jerr);
      m_cinfo.err = &m_jerr;

      jpeg_create_compress(&m_cinfo);

      jpeg_stdio_dest(&m_cinfo, m_out);

      m_cinfo.image_width  = size.width();
      m_cinfo.image_height = size.height();

      m_cinfo.input_components = 3;         /* # of color components per pixel */
      m_cinfo.in_color_space   = JCS_RGB;   /* colorspace of input image */

      jpeg_set_defaults(&m_cinfo);
      //jpeg_set_quality(&m_cinfo, 100, TRUE /* limit to baseline-JPEG values */);

      jpeg_start_compress(&m_cinfo, TRUE);

      std::vector<JSAMPROW> row_pointer(size.height());

      for(int y = 0; y < size.height(); ++y)
      {
        row_pointer[size.height() - y - 1] = reinterpret_cast<JSAMPLE*>(pixels.data() + y*pitch);
      }

      while(m_cinfo.next_scanline < m_cinfo.image_height)
      {
        jpeg_write_scanlines(&m_cinfo, &row_pointer[m_cinfo.next_scanline],
                             size.height() - m_cinfo.next_scanline);
      }

      jpeg_finish_compress(&m_cinfo);

      jpeg_destroy_compress(&m_cinfo);

      fclose(m_out);
    }
  }
  else if ((false))
  { // PNG saving
    FILE* fp = fopen(filename.c_str(), "w");

    if (!fp)
    {
      std::cout << "Error: Couldn't save screenshot: " << strerror(errno) << std::endl;
      return;
    }
    else
    {
      int pitch = size.width() * 3;
      png_structp png_ptr;
      png_infop   info_ptr;

      png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
      info_ptr = png_create_info_struct(png_ptr);

      png_init_io(png_ptr, fp);

      png_set_IHDR(png_ptr, info_ptr,
                   size.width(), size.height(), 8 /* bitdepth */,
                   PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_BASE,
                   PNG_FILTER_TYPE_BASE);

      png_set_compression_level(png_ptr, 0);
      png_write_info(png_ptr, info_ptr);

      std::vector<png_bytep> row_pointers(size.height());

      // generate row pointers
      for (int k = 0; k < size.height(); k++)
        row_pointers[k] = reinterpret_cast<png_byte*>(pixels.data() + ((size.height() - k - 1) * pitch));

      png_write_image(png_ptr, row_pointers.data());

      png_write_end(png_ptr, info_ptr);

      fclose(fp);
    }
  }
}

void
Display::push_framebuffer(FramebufferPtr framebuffer)
{
  framebuffers.push_back(framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.back()->get_handle());
}

void
Display::pop_framebuffer()
{
  assert(!framebuffers.empty());

  framebuffers.pop_back();

  if (!framebuffers.empty())
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.back()->get_handle());
  }
  else
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

FramebufferPtr
Display::get_framebuffer()
{
  if (framebuffers.empty())
    return FramebufferPtr();
  else
    return framebuffers.back();
}

/* EOF */
