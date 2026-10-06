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

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/device.hpp>
#include <wstdisplay/draw_params.hpp>
#include <wstdisplay/framebuffer.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/renderer.hpp>
#include <wstdisplay/surface_manager.hpp>
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

  wstsystem::System system;
  geom::isize window_size(1024, 576);
  auto window = system.create_window("Image Blur", window_size);
  Renderer& renderer = window->get_renderer();
  Canvas canvas;

  SDL_ShowCursor(SDL_DISABLE);

  SurfaceManager surface_manager(window->get_device());

  // the old version used a floating point buffer for the accumulation,
  // wstdisplay only offers RGBA8
  Unique<Framebuffer> framebuffer = window->get_device().create_framebuffer(window_size);

  Surface const surface   = surface_manager.get(argv[1]);
  Surface const surface_2 = surface_manager.get(argv[2]);

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

    canvas.clear();
    canvas.set_blend(Blend::Add);

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

          canvas.draw(surface,
                      DrawParams()
                      .set_scale(1.0f)
                      .set_pos(pos - glm::vec2(surface.get_width()/2, surface.get_height()/2))
                      .set_color(surf::Color(1.0f, 1.0f, 1.0f, n)));
        }
      }
      else
      {
        int n = 32;
        for(int i = 0; i < n; ++i)
        {
          canvas.draw(surface,
                      DrawParams()
                      .set_scale(1.0f)
                      .set_pos((static_cast<float>(i)/static_cast<float>(n-1)) * pos
                               + (static_cast<float>(n-i-1)/static_cast<float>(n-1)) * last_pos
                               - glm::vec2(surface.get_width()/2, surface.get_height()/2))
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
          canvas.draw(surface,
                      DrawParams()
                      .set_scale(scale)
                      .set_pos(glm::vec2(512, 288) - glm::vec2(surface.get_width()/2 * scale,
                                                               surface.get_height()/2 * scale)
                               + (glm::vec2(512, 288) - pos) * scale * 3.0f)
                      .set_color(surf::Color(1.0f, 1.0f, 1.0f, static_cast<float>(1)/static_cast<float>(n))));

        if ((false) && i == 1) // NOLINT
        {
          scale = 1.0f;
          //std::cout << "Black: " << pos << std::endl;
          canvas.set_blend(Blend::Alpha);
          canvas.draw(surface_2,
                      DrawParams()
                      .set_scale(scale)
                      .set_pos(glm::vec2(512, 288) - glm::vec2(surface_2.get_width()/2 * scale,
                                                               surface_2.get_height()/2 * scale)
                               + (glm::vec2(512, 288) - pos) * scale * 3.0f)
                      .set_color(surf::Color(1.0f, 1.0f, 1.0f, 1.0f)));
          canvas.set_blend(Blend::Add);
        }
      }
    }
    renderer.begin_frame(window->get_drawable_size());
    renderer.render(canvas, RenderPass{.target = framebuffer, .clear = surf::Color(0.0f, 0.0f, 0.0f)});

    // show the framebuffer, row 0 of its texture is the top
    canvas.clear();
    canvas.set_space(Space::Clip);
    Vertex const quad[] = {
      {-1.0f,  1.0f, 0.0f, 0.0f},
      { 1.0f,  1.0f, 1.0f, 0.0f},
      { 1.0f, -1.0f, 1.0f, 1.0f},
      {-1.0f, -1.0f, 0.0f, 1.0f},
    };
    canvas.draw_quads(framebuffer->get_texture(), quad);
    renderer.render(canvas);
    renderer.end_frame();

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
