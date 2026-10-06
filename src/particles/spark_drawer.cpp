/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "particles/spark_drawer.hpp"

#include <cmath>

#include <wstdisplay/canvas.hpp>

#include "particles/particle_system.hpp"
#include "util/file_reader.hpp"

namespace windstille {

SparkDrawer::SparkDrawer(ReaderMapping const& props)
  : color(1.0f, 1.0f, 1.0f),
    width(1.0f),
    m_vertices()
{
  props.read("color", color);
  props.read("width", width);
}

void
SparkDrawer::draw(wstdisplay::Canvas& canvas, ParticleSystem const& psys) const
{
  // a quad per spark along its velocity, fading out towards the tail
  wstdisplay::PackedColor const tail = wstdisplay::pack_color(surf::Color(0.0f, 0.0f, 0.0f, 0.0f));
  float const half_width = (width == 1.0f) ? 0.5f : width;

  m_vertices.clear();
  for(ParticleSystem::const_iterator i = psys.begin(); i != psys.end(); ++i)
  {
    float const len = std::sqrt(i->v_x * i->v_x + i->v_y * i->v_y);
    if (len == 0.0f) {
      continue;
    }

    wstdisplay::PackedColor const head = wstdisplay::pack_color(
      surf::Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
    float const o_x = i->v_y / len * half_width;
    float const o_y = i->v_x / len * half_width;
    float const x1 = i->x;
    float const y1 = i->y;
    float const x2 = i->x + i->v_x / 10.0f;
    float const y2 = i->y + i->v_y / 10.0f;

    m_vertices.push_back(wstdisplay::Vertex{x1 + o_x, y1 - o_y, 0.0f, 0.0f, tail, {}});
    m_vertices.push_back(wstdisplay::Vertex{x2 + o_x, y2 - o_y, 0.0f, 0.0f, head, {}});
    m_vertices.push_back(wstdisplay::Vertex{x2 - o_x, y2 + o_y, 0.0f, 0.0f, head, {}});
    m_vertices.push_back(wstdisplay::Vertex{x1 - o_x, y1 + o_y, 0.0f, 0.0f, tail, {}});
  }

  wstdisplay::Canvas::Scope scope(canvas);
  canvas.translate(psys.get_x_pos(), psys.get_y_pos());
  canvas.set_blend(wstdisplay::Blend::Add);
  canvas.draw_quads({}, m_vertices);
}

} // namespace windstille

/* EOF */
