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

#include <geom/rect.hpp>
#include <geom/line.hpp>
#include <geom/quad.hpp>

#include "display/color.hpp"
#include "display/opengl_state.hpp"
#include "display/assert_gl.hpp"

namespace {
std::vector<FramebufferPtr> framebuffers;
} // namespace

GraphicsContext::GraphicsContext() :
  m_aspect_size(),
  m_cliprects()
{
}

GraphicsContext::~GraphicsContext()
{
}

void
GraphicsContext::draw_line(const geom::line& line, const Color& color)
{
  draw_line(line.p1, line.p2, color);
}

void
GraphicsContext::draw_line_with_normal(const geom::line& line, const Color& color)
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
GraphicsContext::draw_line(const glm::vec2& pos1, const glm::vec2& pos2, const Color& color)
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
GraphicsContext::fill_quad(const geom::quad& quad, const Color& color)
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
GraphicsContext::draw_quad(const geom::quad& quad, const Color& color)
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
GraphicsContext::fill_rect(const geom::frect& rect, const Color& color)
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
GraphicsContext::draw_rect(const geom::frect& rect, const Color& color)
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
GraphicsContext::fill_rounded_rect(const geom::frect& rect, float radius, const Color& color)
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
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.top() - y);
    glVertex2f(irect.right() + x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.bottom() + y);
    glVertex2f(irect.right() + x, irect.bottom() + y);
  }
  glEnd();
}

void
GraphicsContext::draw_rounded_rect(const geom::frect& rect, float radius, const Color& color)
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
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.top() - y);
  }
  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.left()  - x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.right() + x, irect.bottom() + y);
  }

  for(int i = 0; i <= n; ++i)
  {
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / static_cast<float>(n)) * radius;

    glVertex2f(irect.right() + x, irect.top() - y);
  }
  // go back to start
  glVertex2f(irect.left(), irect.top() - radius);

  glEnd();
}

void
GraphicsContext::draw_circle(const glm::vec2& pos, float radius, const Color& color, int segments)
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
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;

    glVertex2f(x + pos.x, y + pos.y);
  }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
GraphicsContext::fill_circle(const glm::vec2& pos, float radius, const Color& color, int segments)
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
    float x = cosf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;
    float y = sinf(static_cast<float>(i) * glm::half_pi<float>() / n) * radius;

    glVertex2f(x + pos.x, y + pos.y);
  }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
GraphicsContext::draw_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    draw_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = glm::two_pi<float>() / static_cast<float>(segments);

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
GraphicsContext::fill_arc(const glm::vec2& pos, float radius, float start, float end, const Color& color, int segments)
{
  assert(segments >= 0);

  if (fabsf(end - start) >= 360.0f)
  {
    fill_circle(pos, radius, color, segments);
  }
  else
  {
    float step  = glm::two_pi<float>() / static_cast<float>(segments);

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
GraphicsContext::draw_grid(const glm::vec2& offset, const geom::fsize& size, const Color& rgba)
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

  for(float x = start_x; x < static_cast<float>(m_aspect_size.width()); x += size.width())
  {
    glVertex2f(x, 0);
    glVertex2f(x, static_cast<float>(m_aspect_size.height()));
  }

  for(float y = start_y; y < static_cast<float>(m_aspect_size.height()); y += size.height())
  {
    glVertex2f(0, y);
    glVertex2f(static_cast<float>(m_aspect_size.width()), y);
  }

  glEnd();
}

void
GraphicsContext::push_cliprect(const geom::irect& rect_)
{
  geom::irect rect = rect_;

  if (!m_cliprects.empty())
  {
    rect = geom::irect(std::max(rect.left(), m_cliprects.back().left()),
                std::max(rect.top(),  m_cliprects.back().top()),
                std::min(rect.right(),  m_cliprects.back().right()),
                std::min(rect.bottom(), m_cliprects.back().bottom()));
  }

  m_cliprects.push_back(rect);

  glScissor(rect.left(), size().height() - rect.top() - rect.height(),
            rect.width(), rect.height());
  glEnable(GL_SCISSOR_TEST);
}

void
GraphicsContext::pop_cliprect()
{
  assert(!m_cliprects.empty());

  m_cliprects.pop_back();

  if (!m_cliprects.empty())
  {
    const geom::irect& rect = m_cliprects.back();

    glScissor(rect.left(), size().height() - rect.top() - rect.height(),
              rect.width(), rect.height());
  }
  else
  {
    glDisable(GL_SCISSOR_TEST);
  }
}

void
GraphicsContext::set_aspect_size(geom::isize const& aspect_size)
{
  m_aspect_size = aspect_size;
}

geom::isize
GraphicsContext::size() const
{
  return m_aspect_size;
}

void
GraphicsContext::push_framebuffer(FramebufferPtr framebuffer)
{
  framebuffers.push_back(framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.back()->get_handle());
}

void
GraphicsContext::pop_framebuffer()
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
GraphicsContext::get_framebuffer()
{
  if (framebuffers.empty())
    return FramebufferPtr();
  else
    return framebuffers.back();
}

/* EOF */
