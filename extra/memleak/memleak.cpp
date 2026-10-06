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

#include <surf/software_surface.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/device.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/renderer.hpp>
#include <wstdisplay/surface.hpp>
#include <wstsystem/system.hpp>

using namespace wstdisplay;

namespace {

/** Loads the image given on the command line into a new texture on
    every frame and drops it again, the number of live textures
    printed every 100 frames must stay constant. */
int memleak_main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " IMAGE" << std::endl;
    return EXIT_FAILURE;
  }

  wstsystem::System system;
  auto window = system.create_window("Memleak", geom::isize(800, 600));
  Device& device = window->get_device();
  Renderer& renderer = window->get_renderer();
  Canvas canvas;

  int frame = 0;
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

        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            loop = false;
          }
          break;
      }
    }

    surf::SoftwareSurface const image = surf::SoftwareSurface::from_file(argv[1]);
    Unique<Texture> texture = device.create_texture(image);

    canvas.clear();
    canvas.draw(Surface(texture, image.get_size()), geom::fpoint(0.0f, 0.0f));

    renderer.begin_frame(window->get_drawable_size());
    renderer.render(canvas, RenderPass{.clear = surf::Color(0.0f, 0.0f, 0.0f)});
    texture.reset();
    renderer.end_frame();
    window->swap_buffers();

    if (frame % 100 == 0) {
      std::cout << "frame " << frame << ": " << device.count<Texture>() << " textures" << std::endl;
    }
    frame += 1;

    SDL_Delay(10);
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
    return EXIT_FAILURE;
  } catch(...) {
    std::cerr << "unknown exception" << std::endl;
    return EXIT_FAILURE;
  }
}

/* EOF */
