#include <SDL.h>

#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <geom/io.hpp>
#include <geom/quad.hpp>
#include <surf/palette.hpp>
#include <surf/save.hpp>

#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstsystem/system.hpp>

using namespace wstdisplay;

namespace {

void run()
{
  wstsys::System system;

  std::unique_ptr<OpenGLWindow> window = system.create_window({
      .title = "Simple App",
      .icon = {},
      .size = geom::isize(1280, 720),
      .mode = wstdisplay::OpenGLWindow::Mode::Window,
      .resizable = true,
      .anti_aliasing = 0
    });

  GraphicsContext gc;
  gc.set_ortho(geom::isize(1280, 720));

  bool quit = false;

  system.sig_keyboard_event.connect([&](SDL_KeyboardEvent const& key){
    if (key.state == SDL_PRESSED)
    {
      switch (key.keysym.sym)
      {
        case SDLK_ESCAPE:
          quit = true;
          break;

        case SDLK_f:
          if (window->get_mode() == OpenGLWindow::Mode::Window) {
            if (key.keysym.mod & KMOD_SHIFT) {
              window->set_mode(OpenGLWindow::Mode::Fullscreen);
            } else {
              window->set_mode(OpenGLWindow::Mode::FullscreenDesktop);
            }
          } else {
            window->set_mode(OpenGLWindow::Mode::Window);
          }
          break;
      }
    }
  });

  window->sig_resized.connect([&](geom::isize const& size){
    gc.set_ortho(size);
  });

  while (!quit)
  {
    gc.clear(surf::palette::magenta);
    gc.fill_rect(geom::frect(50, 50, 100, 100), surf::palette::cyan);
    gc.draw_rect(geom::frect(50, 50, 100, 100), surf::palette::red);

    window->swap_buffers();
    system.update();
  }
}

} // namespace

int main(int argc, char** argv)
{
  try {
    run();
    return EXIT_SUCCESS;
  } catch (std::exception const& err) {
    std::cerr << "error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}

/* EOF */
