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

#include <wstdisplay/assert_gl.hpp>
#include <wstdisplay/opengl_state.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/shader_program.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/surface_drawing_parameters.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstsystem/system.hpp>

#include "math/random.hpp"

#pragma GCC diagnostic ignored "-Wold-style-cast"

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

  m_flairs(),

  m_mouse()
{
}

void
Lensflare::draw(GraphicsContext& gc)
{
  glm::vec2 screen_center(static_cast<float>(m_aspect_ratio.width())  / 2.0f,
                          static_cast<float>(m_aspect_ratio.height()) / 2.0f);
  float dist = glm::length(m_mouse - screen_center);

  float factor = 0.3f - (dist / static_cast<float>(m_aspect_ratio.width() + m_aspect_ratio.height()));
  factor *= 3.3f;
  std::cout << factor << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // FIXME: this needs to be moved to a shader
  //glAlphaFunc ( GL_GREATER, 0.5f );
  //glEnable(GL_ALPHA_TEST);

  assert_gl();

  m_cover->draw(gc,
                SurfaceDrawingParameters()
                .set_depth_test(true)
                .set_pos(glm::vec2(600, 400))
                .set_color(surf::Color(0.15f, 0.15f, 0.15f, 1.0f)));

  if ((true))
  {

    GLint samples = 0;
    GLint total_samples = 0;

    GLuint query_id;
    GLuint total_query_id;
    glGenQueries(1, &query_id);
    glGenQueries(1, &total_query_id);

    assert_gl();

    // disable all buffer writing
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // query the number of visible samples
    glBeginQuery(GL_SAMPLES_PASSED, query_id);
    m_lightquery->draw(gc,
                       SurfaceDrawingParameters()
                       .set_depth_test(true)
                       .set_pos(glm::vec2(m_mouse.x - m_lightquery->get_width()/2,
                                          m_mouse.y - m_lightquery->get_height()/2)));
    glEndQuery(GL_SAMPLES_PASSED);

    glGetQueryObjectiv(query_id, GL_QUERY_RESULT, &samples);

    glClear(GL_DEPTH_BUFFER_BIT);

    // reference query, to get the total amount of samples
    glBeginQuery(GL_SAMPLES_PASSED, total_query_id);
    m_lightquery->draw(gc,
                       SurfaceDrawingParameters()
                       .set_pos(glm::vec2(m_mouse.x - m_lightquery->get_width()/2,
                                          m_mouse.y - m_lightquery->get_height()/2)));
    glEndQuery(GL_SAMPLES_PASSED);

    glGetQueryObjectiv(total_query_id, GL_QUERY_RESULT, &total_samples);

    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


    std::cout << "samples: " << samples << " -/- " << total_samples << std::endl;

    glDeleteQueries(1, &query_id);
    glDeleteQueries(1, &total_query_id);

    float visibility = static_cast<float>(samples) / static_cast<float>(total_samples);
    factor *= visibility;

    glDepthMask(GL_FALSE);
    m_halo->draw(gc,
                 SurfaceDrawingParameters()
                 .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                 .set_color(surf::Color(1,1,1,visibility))
                 .set_scale(1.0f)
                 .set_pos(glm::vec2(m_mouse.x,
                                    m_mouse.y)
                          - glm::vec2(m_halo->get_width()/2 * (1.0f),
                                      m_halo->get_height()/2 * (1.0f))));

    glDisable(GL_DEPTH_TEST);
    m_light->draw(gc,
                  SurfaceDrawingParameters()
                  .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                  .set_color(surf::Color(1,1,1,1))
                  .set_scale(visibility)
                  .set_pos(glm::vec2(m_mouse.x - m_light->get_width()/2 * visibility,
                                     m_mouse.y - m_light->get_height()/2 * visibility)));

    glDepthMask(GL_TRUE);

    m_halo->draw(gc,
                 SurfaceDrawingParameters()
                 .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                 .set_color(surf::Color(1,1,1,visibility))
                 .set_scale(2.0f + factor*5.0f)
                 .set_pos(glm::vec2(m_mouse.x,
                                    m_mouse.y)
                          - glm::vec2(m_halo->get_width()/2 * (2.0f + factor*5.0f),
                                      m_halo->get_height()/2 * (2.0f +  factor*5.0f))));


    m_superlight->draw(gc,
                       SurfaceDrawingParameters()
                       .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                       .set_color(surf::Color(1.0f, 1.0f, 1.0f, factor))
                       .set_scale(factor)
                       .set_pos(glm::vec2(m_mouse.x - m_superlight->get_width()/2 * factor,
                                          m_mouse.y - m_superlight->get_height()/2  * factor)));

    for(Flairs::iterator i = m_flairs.begin(); i != m_flairs.end(); ++i)
    {
      i->m_surface->draw(gc,
                         SurfaceDrawingParameters()
                         .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                         .set_scale(i->m_scale)
                         .set_color(surf::Color(i->m_color.r, i->m_color.g, i->m_color.b, i->m_color.a * visibility))
                         .set_pos(screen_center + (m_mouse - screen_center) * i->m_distance
                                  - glm::vec2(i->m_surface->get_width() /2 * i->m_scale,
                                              i->m_surface->get_height()/2 * i->m_scale)));
    }
  }
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
  wstsys::System system;
  auto window = system.create_window("Shader Test", m_window_size);
                                     //m_aspect_ratio, // aspect ratio
                                     //mode = m_fullscreen, // fullscreen
                                     //anti_aliasing = 4}); // anti-alias
  GraphicsContext& gc = window->get_gc();
  TextureManager texture_manager;
  SurfaceManager surface_manager;

  m_light  = surface_manager.get("light.png");
  m_lightquery  = surface_manager.get("lightquery.png");
  m_superlight  = surface_manager.get("superlight.png");
  m_flair1 = surface_manager.get("flair1.png");
  m_flair2 = surface_manager.get("flair2.png");
  m_cover = surface_manager.get("cover.png");
  m_halo = surface_manager.get("halo.png");

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

  GLint query_counter_bits = 0;
  glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_counter_bits);
  if (query_counter_bits == 0)
  {
    std::cout << "Occlusion query not supported" << std::endl;
  }

  m_loop = true;
  while(m_loop)
  {
    glClearColor(0,0,0,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    process_input();
    draw(gc);
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
