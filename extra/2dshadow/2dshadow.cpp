#include <cstdlib>
#include <iostream>

#include <SDL.h>
#include <glm/glm.hpp>

#include <geom/geom.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/draw_params.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/renderer.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstsystem/system.hpp>

using namespace wstdisplay;

int main(int argc, char** argv)
{
  wstsystem::System system;

  geom::isize window_size(854, 480);
  auto window = system.create_window("2D Shadow", window_size);

  Renderer& renderer = window->get_renderer();
  Canvas canvas;

  SurfaceManager surface_manager(window->get_device());

  Surface const darkness = surface_manager.get("darkness.png");
  Surface const light = surface_manager.get("light.png");
  Surface const objects = surface_manager.get("objects.png");
  Surface const shadow  = surface_manager.get("objects_shadow.png");

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

    canvas.clear();

    int samples = 64;
    for(int i = 0; i < samples; ++i)
    {
      float scale = 1.0f + (static_cast<float>(i) / static_cast<float>(samples)) * 2.0f;
      float alpha = static_cast<float>(samples-i) / static_cast<float>(samples);
      alpha = alpha * alpha * alpha * alpha * alpha * alpha;

      //float width  = shadow.get_width() * scale;
      //float height = shadow.get_height() * scale;

      glm::vec2 rel_pos = mouse_pos - object_pos;

      glm::vec2 pos = mouse_pos;

      // pos.x -= shadow.get_width()/2 * scale;
      // pos.y -= shadow.get_height()/2 * scale;

      pos.x -= rel_pos.x * scale;
      pos.y -= rel_pos.y * scale;

      canvas.draw(shadow,
                  DrawParams()
                  .set_pos(pos)
                  .set_scale(glm::vec2(scale, scale))
                  .set_color(surf::Color(1.0f, 1.0f, 1.0f, alpha)));
    }
    canvas.draw(objects, object_pos);
    canvas.set_blend(Blend::Add);
    canvas.draw(light, DrawParams().set_pos(mouse_pos).set_anchor(geom::origin::CENTER));
    canvas.set_blend(Blend::Alpha);
    canvas.draw(darkness, DrawParams().set_pos(mouse_pos).set_anchor(geom::origin::CENTER));

    renderer.begin_frame(window->get_drawable_size());
    renderer.render(canvas, RenderPass{.clear = surf::Color(0.0f, 0.0f, 0.5f)});
    renderer.end_frame();
    window->swap_buffers();
  }

  return EXIT_SUCCESS;
}

/* EOF */


