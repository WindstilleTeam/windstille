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

#include "objects/nightvision.hpp"

#include "display/scene_context.hpp"

#include "app/app.hpp"
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/texture_params.hpp>

#include "math/random.hpp"
#include "util/pathname.hpp"

namespace windstille {

Nightvision::Nightvision(ReaderMapping const& props) :
  nightvision(g_app.sprite().create(Pathname("images/nightvision.sprite"))),
  noise(g_app.texture().get(Pathname("images/noise.png"),
                            wstdisplay::TextureParams{
                              .filter = wstdisplay::TextureFilter::Linear,
                              .wrap_x = wstdisplay::TextureWrap::Repeat,
                              .wrap_y = wstdisplay::TextureWrap::Repeat
                            }).get_texture())
{
  name = "nightvision";
}

Nightvision::~Nightvision()
{
}

void
Nightvision::draw(SceneContext& sc)
{
  // drawn in screen coordinates, with a large z value to stay above
  // everything else
  geom::fsize const size(g_app.window().get_drawable_size());

  {
    wstdisplay::Canvas& light = sc.light();
    wstdisplay::Canvas::Scope scope(light);
    light.set_space(wstdisplay::Space::Screen);

    nightvision.set_alpha(1.0f);
    nightvision.set_blend(wstdisplay::Blend::Opaque);
    nightvision.draw(light, glm::vec2(0, 0), 10000);

    // noise multiplied with the light
    float const u = rnd.frand() / 0.5f;
    float const v = rnd.frand() / 0.5f;
    float const w = 4.0f / 6.0f;
    float const h = 3.0f / 6.0f;
    wstdisplay::Vertex const quad[] = {
      {0.0f, 0.0f, u, v},
      {size.width(), 0.0f, u + w, v},
      {size.width(), size.height(), u + w, v + h},
      {0.0f, size.height(), u, v + h},
    };
    light.set_z(10000);
    light.set_blend(wstdisplay::Blend::Multiply);
    light.draw_quads(noise, quad);
  }

  {
    // FIXME: might be better to copy the highlight over to the
    // color layer
    wstdisplay::Canvas& highlight = sc.highlight();
    highlight.clear();

    wstdisplay::Canvas::Scope scope(highlight);
    highlight.set_space(wstdisplay::Space::Screen);

    nightvision.set_alpha(0.5f);
    nightvision.set_blend(wstdisplay::Blend::Add);
    nightvision.set_scale(std::max(size.width() / nightvision.get_width(),
                                   size.height() / nightvision.get_height()));
    nightvision.draw(highlight,
                     glm::vec2(size.width() / 2.0f - (nightvision.get_width()  * nightvision.get_scale() / 2.0f),
                               size.height() / 2.0f - (nightvision.get_height() * nightvision.get_scale() / 2.0f)),
                     10000);
  }
}

void
Nightvision::update(float )
{
}

} // namespace windstille

/* EOF */
