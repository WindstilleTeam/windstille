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

#include "objects/background_gradient.hpp"

#include <iostream>

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/opengl_window.hpp>

#include "app/app.hpp"
#include "display/scene_context.hpp"

namespace windstille {

BackgroundGradient::BackgroundGradient(ReaderMapping const& props) :
  m_colors()
{
  std::vector<float>& colors = m_colors;

  //props.read("z-pos",  z_pos);
  props.read("colors", colors);

  if (colors.size() % (3 + 4 + 4 + 2) != 0)
  {
    std::cout << "BackgroundGradient: specified color gradient is invalid" << std::endl;
    /** RGBAf gradients are in the format:

        (colors start midpoint end R1 G1 B1 A1 R2 G2 B2 A2 I I
        start midpoint end R1 G1 B1 A1 R2 G2 B2 A2 I I
        ...)

        I is ignored

        all specified in float, this is similar to Gimps gradients
        so you can easily copy&paste
    */
    colors.clear();
  }

}

BackgroundGradient::~BackgroundGradient()
{
}

void
BackgroundGradient::draw(SceneContext& sc)
{
  wstdisplay::Canvas& canvas = sc.color();
  wstdisplay::Canvas::Scope scope(canvas);
  canvas.set_z(-1000.0f);
  canvas.set_space(wstdisplay::Space::Screen);

  // covers the screen, the bands are fractions of its height
  geom::fsize const size(g_app.window().get_drawable_size());
  for(size_t i = 0; i + 13 <= m_colors.size(); i += 13)
  {
    float const start    = m_colors[i + 0];
    float const midpoint = m_colors[i + 1];
    float const end      = m_colors[i + 2];
    surf::Color const color1(m_colors[i + 3], m_colors[i + 4], m_colors[i + 5], m_colors[i + 6]);
    surf::Color const color2(m_colors[i + 7], m_colors[i + 8], m_colors[i + 9], m_colors[i + 10]);
    surf::Color const midcolor((color1.r + color2.r)/2,
                               (color1.g + color2.g)/2,
                               (color1.b + color2.b)/2,
                               (color1.a + color2.a)/2);

    canvas.fill_vertical_gradient(geom::frect(0.0f, start * size.height(), size.width(), midpoint * size.height()),
                                  color1, midcolor);
    canvas.fill_vertical_gradient(geom::frect(0.0f, midpoint * size.height(), size.width(), end * size.height()),
                                  midcolor, color2);
  }
}

} // namespace windstille

/* EOF */
