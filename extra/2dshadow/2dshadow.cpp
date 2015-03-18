#include <iostream>
#include <SDL.h>

#include "system/sdl.hpp"
#include "display/opengl_window.hpp"
#include "display/framebuffer.hpp"
#include "display/surface.hpp"
#include "display/display.hpp"
#include "display/surface_manager.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "math/size.hpp"
#include "math/vector2f.hpp"

int main()
{
  SDL sdl;

  Size window_size(854, 480);
  OpenGLWindow window("2D Shadow", window_size, window_size);

  SurfaceManager surface_manager;

  SurfacePtr darkness = Surface::create(Pathname("darkness.png", Pathname::kSysPath));
  SurfacePtr light = Surface::create(Pathname("light.png", Pathname::kSysPath));
  SurfacePtr objects = Surface::create(Pathname("objects.png", Pathname::kSysPath));
  SurfacePtr shadow  = Surface::create(Pathname("objects_shadow.png", Pathname::kSysPath));

  bool quit = false;
  Vector2f object_pos(100, 0);
  Vector2f mouse_pos;

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
          mouse_pos = Vector2f(static_cast<float>(event.motion.x),
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

      Vector2f rel_pos = mouse_pos - object_pos;

      Vector2f pos = mouse_pos;

      // pos.x -= shadow->get_width()/2 * scale;
      // pos.y -= shadow->get_height()/2 * scale;

      pos.x -= rel_pos.x * scale;
      pos.y -= rel_pos.y * scale;

      shadow->draw(SurfaceDrawingParameters()
                   .set_pos(pos)
                   .set_scale(Vector2f(scale, scale))
                   .set_color(Color(1.0f, 1.0f, 1.0f, alpha)));
    }
    objects->draw(object_pos);
    light->draw(SurfaceDrawingParameters()
                .set_pos(mouse_pos - Vector2f(light->get_width()/2, light->get_height()/2))
                .set_blend_func(GL_SRC_ALPHA, GL_ONE));
    darkness->draw(SurfaceDrawingParameters()
                .set_pos(mouse_pos - Vector2f(darkness->get_width()/2, darkness->get_height()/2)));
    window.swap_buffers();
  }

  return 0;
}

/* EOF */


