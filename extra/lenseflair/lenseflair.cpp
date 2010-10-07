/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#include "lenseflair.hpp"

#include "display/opengl_state.hpp"
#include "display/opengl_window.hpp"
#include "display/shader_program.hpp"
#include "display/surface_manager.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/texture_manager.hpp"
#include "math/random.hpp"
#include "system/sdl.hpp"

Lenseflair::Lenseflair() :
  m_aspect_ratio(1280, 800),
  m_window_size(1280, 800),
  m_fullscreen(false),
  m_loop(false),
  
  m_light(),
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
Lenseflair::draw()
{
  Vector2f screen_center(static_cast<float>(m_aspect_ratio.width)  / 2.0f,
                         static_cast<float>(m_aspect_ratio.height) / 2.0f);
  float dist = (m_mouse - screen_center).length();

  float factor = 0.3f - (dist / static_cast<float>(m_aspect_ratio.width + m_aspect_ratio.height));
  factor *= 3.3f;
  std::cout << factor << std::endl;

  m_light->draw(SurfaceDrawingParameters()
                .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                .set_pos(Vector2f(m_mouse.x - m_light->get_width()/2,
                                  m_mouse.y - m_light->get_height()/2)));

  m_cover->draw(Vector2f(600, 400));

  m_halo->draw(SurfaceDrawingParameters()
                 .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                 .set_scale(2.0f + factor*5.0f)
                 .set_pos(Vector2f(m_mouse.x,
                                   m_mouse.y)
                          - Vector2f(m_halo->get_width()/2 * (2.0f + factor*5.0f),
                                     m_halo->get_height()/2 * (2.0f +  factor*5.0f))));


  m_superlight->draw(SurfaceDrawingParameters()
                     .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                     .set_color(Color(1.0f, 1.0f, 1.0f, factor))
                     .set_scale(factor)
                     .set_pos(Vector2f(m_mouse.x - m_superlight->get_width()/2 * factor,
                                       m_mouse.y - m_superlight->get_height()/2  * factor)));

  for(Flairs::iterator i = m_flairs.begin(); i != m_flairs.end(); ++i)
  {
    i->m_surface->draw(SurfaceDrawingParameters()
                       .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                       .set_scale(i->m_scale)
                       .set_color(Color(1,1,1,factor))
                       .set_pos(screen_center + (m_mouse - screen_center) * i->m_distance
                                - Vector2f(i->m_surface->get_width() /2 * i->m_scale,
                                           i->m_surface->get_height()/2 * i->m_scale)));
  }
  
  if (false)
  {
    m_flair1->draw(SurfaceDrawingParameters()
                   .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                   .set_pos(screen_center + (m_mouse - screen_center) * -0.5f
                            - Vector2f(m_flair1->get_width()/2,
                                       m_flair1->get_height()/2)));
  
    m_flair2->draw(SurfaceDrawingParameters()
                   .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                   .set_pos(screen_center + (m_mouse - screen_center) * -0.75f 
                            - Vector2f(m_flair2->get_width()/2,
                                       m_flair2->get_height()/2)));

    m_flair2->draw(SurfaceDrawingParameters()
                   .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                   .set_pos(screen_center + (m_mouse - screen_center) * 0.25f 
                            - Vector2f(m_flair2->get_width()/2,
                                       m_flair2->get_height()/2)));

    m_flair2->draw(SurfaceDrawingParameters()
                   .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                   .set_pos(screen_center + (m_mouse - screen_center) * 0.75f
                            - Vector2f(m_flair2->get_width()/2,
                                       m_flair2->get_height()/2)));

    m_flair1->draw(SurfaceDrawingParameters()
                   .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                   .set_pos(screen_center + (m_mouse - screen_center) * 0.5f
                            - Vector2f(m_flair1->get_width()/2,
                                       m_flair1->get_height()/2)));
  }
}

void
Lenseflair::process_input()
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
        m_mouse.x = event.motion.x;
        m_mouse.y = event.motion.y;
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
Lenseflair::main()
{
  SDL sdl;
  OpenGLWindow window("Shader Test",
                      m_window_size, // window size
                      m_aspect_ratio, // aspect ratio
                      m_fullscreen, // fullscreen
                      4); // anti-alias
  TextureManager texture_manager;
  SurfaceManager surface_manager;

  m_light  = Surface::create(Pathname("light.png", Pathname::kSysPath));
  m_superlight  = Surface::create(Pathname("superlight.png", Pathname::kSysPath));
  m_flair1 = Surface::create(Pathname("flair1.png", Pathname::kSysPath));
  m_flair2 = Surface::create(Pathname("flair2.png", Pathname::kSysPath));
  m_cover = Surface::create(Pathname("cover.png", Pathname::kSysPath));
  m_halo = Surface::create(Pathname("halo.png", Pathname::kSysPath));

  m_flairs.push_back(Flair(m_flair1, 0.5f, 0.5f));
  m_flairs.push_back(Flair(m_flair1, 1.5f, 0.5f));
  m_flairs.push_back(Flair(m_flair1, -1.5f, 0.5f));
  m_flairs.push_back(Flair(m_flair1, 1.0/2.0f, 0.5f));
  m_flairs.push_back(Flair(m_flair1, 1.0f/8.0f, 0.25f));
  m_flairs.push_back(Flair(m_flair2, -1.0f/2.0f, 0.5f));
  m_flairs.push_back(Flair(m_flair1, -1.0f/3.0f, 0.25f));
  m_flairs.push_back(Flair(m_flair2, -1.0f/4.0f, 0.25f));
  m_flairs.push_back(Flair(m_flair1, -1.0f/5.0f, 0.75f));
  m_flairs.push_back(Flair(m_flair2, -1.0f/5.5f, 0.75f));


  m_loop = true;
  while(m_loop)
  {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    process_input();
    draw();
    window.swap_buffers();
  }

  return 0;
}

int main()
{
  Lenseflair app;
  app.main();
  return 0;
}

/* EOF */
