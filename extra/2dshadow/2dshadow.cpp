#include <iostream>
#include <SDL.h>

#include <glm/glm.hpp>

#include <geom/geom.hpp>

#include "system/sdl.hpp"
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/framebuffer.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/surface_drawing_parameters.hpp>

using namespace wstdisplay;

int main()
{
  SDL sdl;

  geom::isize window_size(854, 480);
  OpenGLWindow window("2D Shadow", window_size, window_size);
  GraphicsContext& gc = window.get_gc();

  SurfaceManager surface_manager;

  wstdisplay::SurfacePtr darkness = surface_manager.get("darkness.png");
  wstdisplay::SurfacePtr light = surface_manager.get("light.png");
  wstdisplay::SurfacePtr objects = surface_manager.get("objects.png");
  wstdisplay::SurfacePtr shadow  = surface_manager.get("objects_shadow.png");

  bool quit = false;
  glm::vec2 object_pos(100, 0);
  glm::vec2 mouse_pos{};

  while(!quit)
  {
    SDL_Event event;

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

        case SDL_MOUSEMOTION:
          mouse_pos = glm::vec2(static_cast<float>(event.motion.x),
                                static_cast<float>(event.motion.y));
          break;
      }
    }

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int samples = 64;
    for(int i = 0; i < samples; ++i)
    {
      float scale = 1.0f + (static_cast<float>(i) / static_cast<float>(samples)) * 2.0f;
      float alpha = static_cast<float>(samples-i) / static_cast<float>(samples);
      alpha = alpha * alpha * alpha * alpha * alpha * alpha;

      //float width  = shadow->get_width() * scale;
      //float height = shadow->get_height() * scale;

      glm::vec2 rel_pos = mouse_pos - object_pos;

      glm::vec2 pos = mouse_pos;

      // pos.x -= shadow->get_width()/2 * scale;
      // pos.y -= shadow->get_height()/2 * scale;

      pos.x -= rel_pos.x * scale;
      pos.y -= rel_pos.y * scale;

      shadow->draw(gc,
                   SurfaceDrawingParameters()
                   .set_pos(pos)
                   .set_scale(glm::vec2(scale, scale))
                   .set_color(surf::Color(1.0f, 1.0f, 1.0f, alpha)));
    }
    objects->draw(gc, object_pos);
    light->draw(gc,
                SurfaceDrawingParameters()
                .set_pos(mouse_pos - glm::vec2(light->get_width()/2, light->get_height()/2))
                .set_blend_func(GL_SRC_ALPHA, GL_ONE));
    darkness->draw(gc, SurfaceDrawingParameters()
                   .set_pos(mouse_pos - glm::vec2(darkness->get_width()/2, darkness->get_height()/2)));
    window.swap_buffers();
  }

  return 0;
}

/* EOF */


