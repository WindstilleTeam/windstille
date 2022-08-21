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

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <SDL.h>
#include <glm/glm.hpp>

#include <glm/glm.hpp>

#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/framebuffer.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/surface_drawing_parameters.hpp>
#include <wstsystem/system.hpp>

using namespace wstdisplay;

namespace {

int app_main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " FILENAME FILENAME" << std::endl;
    return -1;
  }

  wstsys::System system;
  geom::isize window_size(1024, 576);
  auto window = system.create_window("Image Blur", window_size);
  GraphicsContext& gc = window->get_gc();

  SDL_ShowCursor(SDL_DISABLE);

  SurfaceManager surface_manager;

  FramebufferPtr framebuffer = Framebuffer::create_hdr(window_size);

  wstdisplay::SurfacePtr surface   = surface_manager.get(argv[1]);
  wstdisplay::SurfacePtr surface_2 = surface_manager.get(argv[2]);

  float ray_length = 3.0f;
  glm::vec2 pos{};
  glm::vec2 last_pos{};
  int t = 0;
  std::vector<glm::vec2> buffer(16);
  std::vector<glm::vec2>::size_type buffer_pos = 0;
  bool quit = false;
  while(!quit)
  {
    SDL_Event event;
    last_pos = pos;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          // FIXME: This should be a bit more gentle, but will do for now
          std::cout << "Ctrl-c or Window-close pressed, game is going to quit" << std::endl;
          quit = true;
          break;

        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
          {
            quit = true;
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == 1)
          {
            ray_length *= 1.0f/1.4f;
          }
          else if (event.button.button == 3)
          {
            ray_length *= 1.4f;
          }
          std::cout << ray_length << std::endl;
          break;

        case SDL_MOUSEMOTION:
          //std::cout << event.motion.x << ", " << event.motion.y << std::endl;
          last_pos = pos;
          pos = glm::vec2(1024.0f - static_cast<float>(event.motion.x),
                          576.0f - static_cast<float>(event.motion.y));
          break;

        default:
          break;
      }
    }

    t += 30;
    //ray_length = sin(t/1000.0f);

    buffer[buffer_pos % buffer.size()] = pos;
    buffer_pos += 1;

    gc.push_framebuffer(framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ((true))
    {
      if ((false))
      {
        for(size_t i = 0; i < std::min(buffer_pos, buffer.size()); ++i)
        {
          size_t idx = (buffer_pos - buffer.size() + i) % buffer.size();
          pos = buffer[idx];

          float n = static_cast<float>(buffer.size());

          if ((false))
          { // after image motion blur
            n = static_cast<float>(i) / ((n * n + n) / 2.0f);
          }
          else
          { // simple trail, doesn't fade out
            n = 1.0f / n;
          }

          surface->draw(gc,
                        SurfaceDrawingParameters()
                        .set_scale(1.0f)
                        .set_pos(pos - glm::vec2(surface->get_width()/2, surface->get_height()/2))
                        .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                        .set_color(surf::Color(1.0f, 1.0f, 1.0f, n)));
        }
      }
      else
      {
        int n = 32;
        for(int i = 0; i < n; ++i)
        {
          surface->draw(gc,
                        SurfaceDrawingParameters()
                        .set_scale(1.0f)
                        .set_pos((static_cast<float>(i)/static_cast<float>(n-1)) * pos
                                 + (static_cast<float>(n-i-1)/static_cast<float>(n-1)) * last_pos
                                 - glm::vec2(surface->get_width()/2, surface->get_height()/2))
                        .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                        .set_color(surf::Color(1.0f, 1.0f, 1.0f, 1.0f / static_cast<float>(n))));
        }
      }
    }
    else
    {
      int n = 100;
      for(int i = 0; i < n; ++i)
      {
        float scale = 1.0f + static_cast<float>(i) / static_cast<float>(n) * ray_length;
        if ((true))
          surface->draw(gc,
                        SurfaceDrawingParameters()
                        .set_scale(scale)
                        .set_pos(glm::vec2(512, 288) - glm::vec2(surface->get_width()/2 * scale,
                                                                 surface->get_height()/2 * scale)
                                 + (glm::vec2(512, 288) - pos) * scale * 3.0f)
                        .set_blend_func(GL_SRC_ALPHA, GL_ONE)
                        .set_color(surf::Color(1.0f, 1.0f, 1.0f, static_cast<float>(1)/static_cast<float>(n))));

        if ((false) && i == 1) // NOLINT
        {
          scale = 1.0f;
          //std::cout << "Black: " << pos << std::endl;
          surface_2->draw(gc,
                          SurfaceDrawingParameters()
                          .set_scale(scale)
                          .set_pos(glm::vec2(512, 288) - glm::vec2(surface_2->get_width()/2 * scale,
                                                                   surface_2->get_height()/2 * scale)
                                   + (glm::vec2(512, 288) - pos) * scale * 3.0f)
                          .set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
                          .set_color(surf::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        }
      }
    }
    gc.pop_framebuffer();

    if ((true))
    {
      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer->get_handle());
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

      glBlitFramebufferEXT(0, 0, framebuffer->get_width(), framebuffer->get_height(),
                           0, 0, framebuffer->get_width(), framebuffer->get_height(),
                           GL_COLOR_BUFFER_BIT, GL_LINEAR /*NEAREST*/);

      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    }

    window->swap_buffers();
    SDL_Delay(20);
  }

  return 0;
}

} // namespace

int main(int argc, char** argv)
{
  try {
    return app_main(argc, argv);
  } catch(std::exception const& err) {
    std::cerr << "exception: " << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch(...) {
    std::cerr << "unknown exception" << std::endl;
    return EXIT_FAILURE;
  }
}

/* EOF */
