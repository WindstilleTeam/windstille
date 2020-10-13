/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "slide_object.hpp"

#include <iostream>
#include <math.h>

#include "plugins/jpeg.hpp"
#include "display/surface_drawing_parameters.hpp"

SlideObject::SlideObject(std::filesystem::path const& filename) :
  m_filename(filename),
  m_size(0.0f, 0.0f),
  m_surface(),
  m_begin(0.0f),
  m_path(),
  m_fade_in_time(0.0f),
  m_fade_out_time(0.0f)
{
  m_size = Sizef(JPEG::get_size(filename));
}

void
SlideObject::set_fade_in(float f)
{
  m_fade_in_time = f;
}

void
SlideObject::set_fade_out(float f)
{
  m_fade_out_time = f;
}

void
SlideObject::draw(float relative_time)
{
  if (!m_surface)
  {
    m_surface = Surface::create(m_filename);
  }

  SlidePathNode node = m_path.get(relative_time);

  // FIXME: hardcoded fade hack
  Color color(1.0f, 1.0f, 1.0f, 1.0f);
  if (relative_time < m_fade_in_time)
  {
    color.a = relative_time / m_fade_in_time;
  }
  else if (relative_time + m_fade_out_time > length())
  {
    color.a = (length() - relative_time) / m_fade_out_time;
  }

  Vector2f pos(node.pos);

  // zoom needs to grow exponentially to be linear
  float scale = node.zoom;

  // recalc pos so that we are draw the image centered
  pos.x -= m_surface->get_width() /2.0f * scale;
  pos.y -= m_surface->get_height()/2.0f * scale;

  m_surface->draw(SurfaceDrawingParameters()
                  .set_color(color)
                  .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                  .set_pos(pos)
                  .set_scale(scale));
}

float
SlideObject::length() const
{
  return m_path.length();
}

float
SlideObject::begin() const
{
  return m_begin;
}

float
SlideObject::end() const
{
  return begin() + length();
}

void
SlideObject::set_begin(float beg)
{
  m_begin = beg;
}

float
SlideObject::get_width() const
{
  return m_size.width();
}

float
SlideObject::get_height() const
{
  return m_size.height();
}

std::filesystem::path
SlideObject::get_filename() const
{
  return m_filename;
}

bool
SlideObject::unload()
{
  if (m_surface)
  {
    m_surface.reset();
    std::cout << "Unloading: " << m_filename << std::endl;
    return true;
  }
  else
  {
    return false;
  }
}

/* EOF */
