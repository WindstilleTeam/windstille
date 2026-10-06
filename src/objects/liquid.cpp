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

#include "objects/liquid.hpp"

#include <cmath>
#include <numbers>
#include <utility>

#include "app/app.hpp"
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/texture_params.hpp>

#include "display/scene_context.hpp"
#include "util/pathname.hpp"

#define SAMPLES 5

namespace windstille {


Liquid::Liquid(ReaderMapping const& props) :
  texture(),
  t(),
  heightfield_store1(),
  heightfield_store2(),
  heightfield1(),
  heightfield2(),
  m_vertices()
{
  int width = 10;
  t = 0;

  props.read("pos",    pos);
  props.read("width",  width);

  heightfield1 = &heightfield_store1;
  heightfield2 = &heightfield_store2;

  heightfield1->resize(width * SAMPLES, 0);
  heightfield2->resize(width * SAMPLES, 0);

  for(std::vector<float>::size_type i = 2; i < heightfield1->size()-2; ++i)
  {
    (*heightfield1)[i] = sinf(static_cast<float>(i) / static_cast<float>(heightfield1->size()) * 10.0f) * 0.5f
      + sinf(static_cast<float>(i) / static_cast<float>(heightfield1->size()) * 5.0f) * .5f;
    (*heightfield2)[i] = (*heightfield1)[i];
  }

  for(int i = 50; i < 70 && i < int(heightfield1->size()); ++i)
    (*heightfield1)[i] += 0.0025f;

  texture = g_app.texture().get(Pathname("images/textures/water.png"),
                                wstdisplay::TextureParams{
                                  .wrap_x = wstdisplay::TextureWrap::Repeat,
                                  .wrap_y = wstdisplay::TextureWrap::Repeat
                                }).get_texture();
}

Liquid::~Liquid()
{
}

void
Liquid::update(float delta)
{
  { // Update the heightfield data
    t += delta * 1.0f;
    float factor = 0.1f * delta;

    for(int samples = 0; samples < 3; ++samples)
    {
      for(std::vector<float>::size_type i = 2; i < heightfield1->size()-2; ++i)
      {
        float value =
          factor * ((*heightfield1)[i-2] +
                    (*heightfield1)[i-1] +
                    (*heightfield1)[i+1] +
                    (*heightfield1)[i+2])
          - (factor * 4 * (*heightfield1)[i])
          + (2*(*heightfield1)[i])
          - (*heightfield2)[i];

        (*heightfield2)[i] = value * 0.99999f;
      }
      std::swap(heightfield2, heightfield1);
    }
  }
}

void
Liquid::draw(SceneContext& sc)
{
  float const texscale = 1.0f/128.0f;
  std::vector<float> const& heights = *heightfield1;

  // the surface and the body as strips of quads between neighboring
  // samples, the vertices of a column are top then bottom
  auto add_strip = [this, &heights](auto&& column) {
    for(size_t i = 1; i < heights.size(); ++i)
    {
      auto const [top1, bottom1] = column(i - 1);
      auto const [top2, bottom2] = column(i);
      m_vertices.push_back(top1);
      m_vertices.push_back(top2);
      m_vertices.push_back(bottom2);
      m_vertices.push_back(bottom1);
    }
  };

  auto vertex = [&](size_t i, float y, surf::Color const& color) {
    float const x = static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES);
    float const wobble = std::sin(t + static_cast<float>(i)/10.0f) * 0.2f;
    return wstdisplay::Vertex{x, y, x * texscale + wobble, y * texscale + wobble,
                              wstdisplay::pack_color(color), {}};
  };

  m_vertices.clear();

  // water top
  add_strip([&](size_t i) {
    float c = 0.5f;
    if (i > 0)
    {
      float angle = std::atan2(32.0f * (heights[i] - heights[i-1]), 3.2f);
      c = std::min(1.0f, std::max(0.5f, 8.0f * (angle/std::numbers::pi_v<float>) + 0.5f));
    }
    float const y = -32.0f * heights[i];
    return std::pair(vertex(i, y, surf::Color(c, c, 1.0f, 1.0f)),
                     vertex(i, y + 8.0f, surf::Color(0.5f, 0.5f, 1.0f, 0.7f)));
  });

  // water body
  add_strip([&](size_t i) {
    return std::pair(vertex(i, -32.0f * heights[i] + 8.0f, surf::Color(0.5f, 0.5f, 1.0f, 0.7f)),
                     vertex(i, 64.0f, surf::Color(0.0f, 0.0f, 0.5f, 0.7f)));
  });

  wstdisplay::Canvas& canvas = sc.color();
  wstdisplay::Canvas::Scope scope(canvas);
  canvas.set_z(10000.0f);
  canvas.translate(pos.x, pos.y);
  canvas.draw_quads(texture, m_vertices);
}

} // namespace windstille

/* EOF */
