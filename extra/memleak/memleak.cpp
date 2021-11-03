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

#include <SDL.h>
#include <sstream>
#include <stdexcept>

#include <argparser.hpp>

#include <wstdisplay/assert_gl.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/framebuffer.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/surface_drawing_parameters.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstsystem/system.hpp>

#include "util/system.hpp"

using namespace wstdisplay;

namespace {

int memleak_main(int argc, char* argv[])
{
  wstsys::System system;
  auto window = system.create_window("Memleak", geom::isize(800, 600));
  GraphicsContext& gc = window->get_gc();

  TextureManager    texture_manager;
  SurfaceManager    surface_manager;

  wstdisplay::SurfacePtr surface;

  bool loop = true;
  while(loop)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          loop = false;
          break;

        case SDL_KEYUP:
          break;

        case SDL_KEYDOWN:
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              loop = false;
              break;

            default:
              break;
          }
          break;
      }
    }

    surface = surface_manager.get(argv[1]);

    surface->draw(gc, glm::vec2(0.0f, 0.0f));
    window->swap_buffers();
    SDL_Delay(10);

    surface.reset();
    surface_manager.cleanup();
    texture_manager.cleanup();
  }

  return 0;
}

} // namespace

int main(int argc, char** argv)
{
  try {
    return memleak_main(argc, argv);
  } catch(std::exception const& err) {
    std::cerr << "exception: " << err.what() << std::endl;
  } catch(...) {
    std::cerr << "unknown exception" << std::endl;
  }
}

/* EOF */
