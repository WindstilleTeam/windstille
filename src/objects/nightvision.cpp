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

#include "app/app.hpp"
#include "display/graphics_context.hpp"
#include "display/opengl_window.hpp"
#include "display/texture_manager.hpp"
#include "math/random.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "util/pathname.hpp"

Nightvision::Nightvision(ReaderMapping const& props) :
  nightvision(g_app.sprite().create(Pathname("images/nightvision.sprite"))),
  noise(g_app.texture().get(Pathname("images/noise.png")))
{
  name = "nightvision";
  noise->set_wrap(GL_REPEAT);
  noise->set_filter(GL_LINEAR);
}

Nightvision::~Nightvision()
{
}

void
Nightvision::draw(SceneContext& sc)
{
  // reset the modelview, so we can draw in screen coordinates
  sc.light().push_modelview();
  sc.light().set_modelview(glm::mat4(1.0f));

  // try to stay above everything else with large z value
  if (1)
  {
    nightvision.set_alpha(1.0f);
    nightvision.set_blend_func(GL_ONE, GL_ZERO);
    nightvision.draw(sc.light(), glm::vec2(0, 0), 10000);
  }

  if (1)
  {
    auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0), 10000,
                                                       sc.light().get_modelview());
    array->set_mode(GL_QUADS);
    array->set_texture(noise);
    array->set_blend_func(GL_DST_COLOR, GL_ZERO);

    float u = rnd.frand() / 0.5f;
    float v = rnd.frand() / 0.5f;
    float w = 4.0f / 6.0f;
    float h = 3.0f / 6.0f;

    array->texcoord(u, v);
    array->vertex(0, 0);

    array->texcoord(u + w, v);
    array->vertex(static_cast<float>(g_app.window().get_gc().size().width()), 0);

    array->texcoord(u + w, v + h);
    array->vertex(static_cast<float>(g_app.window().get_gc().size().width()), static_cast<float>(g_app.window().get_gc().size().height()));

    array->texcoord(u, v + h);
    array->vertex(0, static_cast<float>(g_app.window().get_gc().size().height()));

    if ((false)) // second noise level
    {
      u = rnd.frand();
      v = rnd.frand();
      float size = 4.0f;

      array->texcoord(u, v);
      array->vertex(0, 0, 1.0f);

      array->texcoord(u + size, v);
      array->vertex(static_cast<float>(g_app.window().get_gc().size().width()), 0.0f, 1.0f);

      array->texcoord(u + size, v + size);
      array->vertex(static_cast<float>(g_app.window().get_gc().size().width()), static_cast<float>(g_app.window().get_gc().size().height()), 1.0f);

      array->texcoord(u, v + size);
      array->vertex(0.0f, static_cast<float>(g_app.window().get_gc().size().height()), 1.0f);
    }

    sc.light().draw(std::move(array));
  }
  sc.light().pop_modelview();

  if (1)
  {
    // FIXME: might be better to copy the highlight over to the
    // color layer, however that would require some changes to the
    // DrawingContext structure
    sc.highlight().clear();

    sc.highlight().push_modelview();
    sc.highlight().set_modelview(glm::mat4(1.0f));

    nightvision.set_alpha(0.5f);
    nightvision.set_blend_func(GL_SRC_ALPHA, GL_ONE);
    nightvision.set_scale(std::max(float(g_app.window().get_gc().size().width())  / nightvision.get_width(),
                                   float(g_app.window().get_gc().size().height()) / nightvision.get_height()));

    nightvision.draw(sc.highlight(),
                     glm::vec2(static_cast<float>(g_app.window().get_gc().size().width()) / 2.0f - (nightvision.get_width()  * nightvision.get_scale() / 2.0f),
                               static_cast<float>(g_app.window().get_gc().size().height())/ 2.0f - (nightvision.get_height() * nightvision.get_scale() / 2.0f)),
                     10000);
    sc.highlight().pop_modelview();
  }
}

void
Nightvision::update(float )
{
}

/* EOF */
