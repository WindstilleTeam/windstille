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

#include "display/assert_gl.hpp"
#include "display/opengl_window.hpp"
#include "display/framebuffer.hpp"
#include "display/surface_manager.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/surface.hpp"
#include "display/display.hpp"
#include "display/texture_manager.hpp"
#include "util/system.hpp"

namespace {

int memleak_main(int argc, char* argv[])
{
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;

  if (SDL_Init(flags) < 0)
  {
    std::ostringstream msg;
    msg << "Couldn't initialize SDL: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    atexit(SDL_Quit);
  }


  //std::cout << "OpenGLWindow" << std::endl;
  OpenGLWindow window("Memleak",
                      Size(800, 600), // window size
                      Size(800, 600),
                      false,
                      4); // anti-alias

  TextureManager    texture_manager;
  SurfaceManager    surface_manager;

  SurfacePtr surface;

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

    surface = Surface::create(argv[1]);

    surface->draw(glm::vec2(0.0f, 0.0f));
    window.swap_buffers();
    SDL_Delay(10);

    surface.reset();
    SurfaceManager::current()->cleanup();
    TextureManager::current()->cleanup();
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
