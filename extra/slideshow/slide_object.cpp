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
#include <surf/software_surface.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/draw_params.hpp>

using namespace wstdisplay;

SlideObject::SlideObject(std::filesystem::path const& filename,
                         wstdisplay::Device& device) :
  m_filename(filename),
  m_device(device),
  m_size(0.0f, 0.0f),
  m_texture(),
  m_surface(),
  m_begin(0.0f),
  m_path(),
  m_fade_in_time(0.0f),
  m_fade_out_time(0.0f)
{
  m_size = geom::fsize(JPEG::get_size(filename.string()));
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
SlideObject::draw(Canvas& canvas, float relative_time)
{
  if (!m_texture)
  {
    surf::SoftwareSurface const image = surf::SoftwareSurface::from_file(m_filename);
    m_texture = m_device.create_texture(image);
    m_surface = Surface(m_texture, image.get_size());
  }

  SlidePathNode node = m_path.get(relative_time);

  // FIXME: hardcoded fade hack
  surf::Color color(1.0f, 1.0f, 1.0f, 1.0f);
  if (relative_time < m_fade_in_time)
  {
    color.a = relative_time / m_fade_in_time;
  }
  else if (relative_time + m_fade_out_time > length())
  {
    color.a = (length() - relative_time) / m_fade_out_time;
  }

  glm::vec2 pos(node.pos);

  // zoom needs to grow exponentially to be linear
  float scale = node.zoom;

  Canvas::Scope scope(canvas);
  canvas.set_blend(Blend::Add);
  canvas.draw(m_surface,
              DrawParams()
              .set_color(color)
              .set_pos(pos)
              .set_anchor(geom::origin::CENTER)
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
  if (m_texture)
  {
    m_texture.reset();
    m_surface = {};
    std::cout << "Unloading: " << m_filename << std::endl;
    return true;
  }
  else
  {
    return false;
  }
}

/* EOF */
