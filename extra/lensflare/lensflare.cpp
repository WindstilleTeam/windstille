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

#include "lensflare.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/draw_params.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/renderer.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstsystem/system.hpp>

using namespace wstdisplay;

Lensflare::Lensflare() :
  m_aspect_ratio(1280, 800),
  m_window_size(640, 400),
  m_fullscreen(false),
  m_loop(false),

  m_light(),
  m_lightquery(),
  m_superlight(),
  m_flair1(),
  m_flair2(),
  m_cover(),
  m_halo(),

  m_lightquery_image(),
  m_cover_image(),
  m_cover_pos(600, 400),

  m_flairs(),

  m_mouse()
{
}

float
Lensflare::get_visibility() const
{
  // replaces an occlusion query: the pixels of the light query image
  // that aren't covered by an opaque pixel of the cover
  glm::vec2 const origin = m_mouse - glm::vec2(m_lightquery.get_width() / 2, m_lightquery.get_height() / 2);
  int total = 0;
  int visible = 0;
  for (int y = 0; y < m_lightquery_image.get_height(); y += 2) {
    for (int x = 0; x < m_lightquery_image.get_width(); x += 2) {
      if (m_lightquery_image.get_pixel(geom::ipoint(x, y)).a < 0.5f) {
        continue;
      }
      total += 1;

      glm::ivec2 const c(glm::floor(origin + glm::vec2(x, y) - m_cover_pos));
      bool const covered = (c.x >= 0 && c.y >= 0 &&
                            c.x < m_cover_image.get_width() && c.y < m_cover_image.get_height() &&
                            m_cover_image.get_pixel(geom::ipoint(c.x, c.y)).a >= 0.5f);
      if (!covered) {
        visible += 1;
      }
    }
  }
  return total > 0 ? static_cast<float>(visible) / static_cast<float>(total) : 0.0f;
}

void
Lensflare::draw(Canvas& canvas)
{
  glm::vec2 screen_center(static_cast<float>(m_aspect_ratio.width())  / 2.0f,
                          static_cast<float>(m_aspect_ratio.height()) / 2.0f);
  float dist = glm::length(m_mouse - screen_center);

  float factor = 0.3f - (dist / static_cast<float>(m_aspect_ratio.width() + m_aspect_ratio.height()));
  factor *= 3.3f;

  canvas.draw(m_cover,
              DrawParams()
              .set_pos(m_cover_pos)
              .set_color(surf::Color(0.15f, 0.15f, 0.15f, 1.0f)));

  float const visibility = get_visibility();
  factor *= visibility;

  canvas.set_blend(Blend::Add);

  // the halo and the light are partly behind the cover
  canvas.draw(m_halo,
              DrawParams()
              .set_color(surf::Color(1, 1, 1, visibility))
              .set_pos(m_mouse)
              .set_anchor(geom::origin::CENTER));

  canvas.draw(m_light,
              DrawParams()
              .set_scale(visibility)
              .set_pos(m_mouse)
              .set_anchor(geom::origin::CENTER));

  canvas.draw(m_halo,
              DrawParams()
              .set_color(surf::Color(1, 1, 1, visibility))
              .set_scale(2.0f + factor * 5.0f)
              .set_pos(m_mouse)
              .set_anchor(geom::origin::CENTER));

  canvas.draw(m_superlight,
              DrawParams()
              .set_color(surf::Color(1.0f, 1.0f, 1.0f, factor))
              .set_scale(factor)
              .set_pos(m_mouse)
              .set_anchor(geom::origin::CENTER));

  for(Flairs::iterator i = m_flairs.begin(); i != m_flairs.end(); ++i)
  {
    canvas.draw(i->m_surface,
                DrawParams()
                .set_scale(i->m_scale)
                .set_color(surf::Color(i->m_color.r, i->m_color.g, i->m_color.b, i->m_color.a * visibility))
                .set_pos(screen_center + (m_mouse - screen_center) * i->m_distance)
                .set_anchor(geom::origin::CENTER));
  }

  canvas.set_blend(Blend::Alpha);
}

void
Lensflare::process_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
        m_loop = false;
        break;

      case SDL_MOUSEMOTION:
        m_mouse.x = static_cast<float>(m_aspect_ratio.width()  * event.motion.x / m_window_size.width());
        m_mouse.y = static_cast<float>(m_aspect_ratio.height() * event.motion.y / m_window_size.height());
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            m_loop = false;
            break;

          default:
            break;
        }
        break;

      default:
        break;
    }
  }
}

int
Lensflare::run()
{
  wstsystem::System system;
  auto window = system.create_window("Lensflare", m_window_size);
  Renderer& renderer = window->get_renderer();
  Canvas canvas;
  SurfaceManager surface_manager(window->get_device());

  m_light  = surface_manager.get("light.png");
  m_lightquery  = surface_manager.get("lightquery.png");
  m_superlight  = surface_manager.get("superlight.png");
  m_flair1 = surface_manager.get("flair1.png");
  m_flair2 = surface_manager.get("flair2.png");
  m_cover = surface_manager.get("cover.png");
  m_halo = surface_manager.get("halo.png");

  m_lightquery_image = surf::SoftwareSurface::from_file("lightquery.png");
  m_cover_image = surf::SoftwareSurface::from_file("cover.png");

  float pos[] = { 0.1f, 0.2f, 0.4f, 0.8f, 1.6f, 3.2f };

  for(size_t i = 0; i < sizeof(pos)/sizeof(float); ++i)
  {
    m_flairs.push_back(Flair(m_flair2, pos[i], 1.0f * pos[i], surf::Color(1,1,1,1)));
    m_flairs.push_back(Flair(m_flair2, -pos[i] * 0.7f, 1.0f * pos[i] * 0.7f, surf::Color(1,1,1,1)));
  }

  for(size_t i = 0; i < sizeof(pos)/sizeof(float); ++i)
  {
    m_flairs.push_back(Flair(m_flair1, pos[i]* 0.2f, 1.0f * pos[i] * 0.2f, surf::Color(1,1,1,0.1f)));
    m_flairs.push_back(Flair(m_flair1, -pos[i] * 0.1f, 1.0f * pos[i] * 0.1f, surf::Color(1,1,1,0.1f)));
  }

  m_loop = true;
  while(m_loop)
  {
    process_input();

    canvas.clear();
    draw(canvas);

    // everything is placed in m_aspect_ratio coordinates
    geom::isize const size = window->get_drawable_size();
    glm::mat4 const scale = glm::scale(glm::mat4(1.0f),
                                       glm::vec3(static_cast<float>(size.width()) / static_cast<float>(m_aspect_ratio.width()),
                                                 static_cast<float>(size.height()) / static_cast<float>(m_aspect_ratio.height()),
                                                 1.0f));
    renderer.begin_frame(size);
    renderer.render(canvas, RenderPass{.clear = surf::Color(0.0f, 0.0f, 0.0f), .world_matrix = scale});
    renderer.end_frame();
    window->swap_buffers();
  }

  return 0;
}

int main(int argc, char* argv[])
{
  try
  {
    Lensflare app;
    return app.run();
  }
  catch(std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}

/* EOF */
