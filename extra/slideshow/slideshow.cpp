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

#include <SDL.h>

#include <sstream>
#include <stdexcept>

#include "display/opengl_window.hpp"
#include "display/surface_manager.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/surface.hpp"
#include "display/texture_manager.hpp"
#include "util/system.hpp"

#include "slideshow/slide_show.hpp"
#include "slideshow/slide_builder.hpp"

void
init_sdl()
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
    SDL_EnableUNICODE(1);
  }
}

int main(int argc, char** argv)
{
  if (argc < 1)
  {
    std::cerr << "Usage: " << argv[0] << " FILE..." << std::endl;
    return 0;
  }
  else
  {
    try 
    {
      init_sdl();
      
      Size aspect(640, 480);
      //std::cout << "OpenGLWindow" << std::endl;
      OpenGLWindow window(Size(640, 480), // window size
                          aspect, // aspect ratio
                          false, // fullscreen
                          4); // anti-alias

      TextureManager    texture_manager;
      SurfaceManager    surface_manager;

      SlideShow slide_show;
      
      { // read from file
        SlideBuilder slide_builder(slide_show, aspect);
        for(int i = 1; i < argc; ++i)
        {
          //std::cout << "Reading " << argv[i] << std::endl;
          slide_builder.load_from_file(argv[i]);
        }
      }

      // Surface surface1(Pathname(argv[1], Pathname::kSysPath));
      // Surface surface2(Pathname(argv[2], Pathname::kSysPath));
  
      bool loop = true;
      //float progress = 0.0f;

      Uint32 last_ticks = SDL_GetTicks();
      while(loop && !slide_show.done())
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
            case SDL_KEYUP:
              if (event.key.state)
              {    
                switch (event.key.keysym.sym)
                {
                  case SDLK_F6:
                    break;

                  default:
                    break;
                }
              }
                     
              return 0;
          }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Uint32 ticks = SDL_GetTicks();
        slide_show.update(static_cast<float>(ticks - last_ticks) / 1000.0f);
        last_ticks = ticks;
        slide_show.draw();

        // surface1.draw(SurfaceDrawingParameters()
        //               .set_blend_func(GL_SRC_ALPHA, GL_ONE)
        //               .set_color(Color(1.0f, 1.0f, 1.0f, 1.0f - progress))
        //               .set_scale(1.0f)
        //               .set_pos(Vector2f(0.0f, 0.0f)));
    
        // surface2.draw(SurfaceDrawingParameters()
        //               .set_blend_func(GL_SRC_ALPHA, GL_ONE)
        //               .set_color(Color(1.0f, 1.0f, 1.0f, progress))
        //               .set_scale(1.0f + progress)
        //               .set_pos(Vector2f(0.0f - (100.0f * progress),
        //                                 0.0f)));

        SDL_GL_SwapBuffers();

        SDL_Delay(10);
      }
    }
    catch(std::exception& err)
    {
      std::cout << err.what() << std::endl;
      return 1;
    }
    return 0;
  }
}

/* EOF */
