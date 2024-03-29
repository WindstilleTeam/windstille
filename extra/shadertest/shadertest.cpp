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

#include "shadertest.hpp"

#include <iostream>

#include <wstdisplay/opengl_state.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/shader_program.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>
#include <wstsystem/system.hpp>

#include "math/random.hpp"

using namespace wstdisplay;

App::App() :
  m_aspect_ratio(1280, 800),
  m_window_size(1280, 800),
  m_fullscreen(false)
{
}

int
App::run(int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] << " IMAGETEX DISPLACETEX COLORTEX VERT FRAG" << std::endl;
    return EXIT_FAILURE;
  }

  wstsys::System system;
  auto window = system.create_window("Shader Test", m_window_size);
                                     //m_aspect_ratio, // aspect ratio
                                     //m_fullscreen, // fullscreen
                                     //4); // anti-alias
  TextureManager texture_manager;
  SurfaceManager surface_manager;

  wstdisplay::SurfacePtr image_surface = surface_manager.get(argv[1]);
  wstdisplay::SurfacePtr displace_surface = surface_manager.get(argv[2]);
  wstdisplay::SurfacePtr color_surface = surface_manager.get(argv[3]);

  ShaderProgramPtr prog = ShaderProgram::create();

  prog->attach(ShaderObject::from_file(GL_VERTEX_SHADER, argv[4]));
  prog->attach(ShaderObject::from_file(GL_FRAGMENT_SHADER, argv[5]));
  prog->link();

  glm::vec2 displacement(0.0f, 0.0f);

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
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              loop = false;
              break;

            default:
              break;
          }
          break;

        case SDL_MOUSEMOTION:
          displacement.x = static_cast<float>(event.motion.x - 640) / 1280.0f;
          displacement.y = static_cast<float>(event.motion.x - 400) / 800.0f;
          break;

        default:
          break;
      }
    }

    {
      glBindTexture(GL_TEXTURE_2D, image_surface->get_texture()->get_handle());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

      glBindTexture(GL_TEXTURE_2D, displace_surface->get_texture()->get_handle());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

      glBindTexture(GL_TEXTURE_2D, color_surface->get_texture()->get_handle());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }

    {
      glUseProgram(prog->get_handle());

      // uniforms are optimized away when not used, thus giving error
      prog->set_uniform1i("image_tex", 0);
      prog->set_uniform1i("displace_tex", 1);
      //prog->set_uniform1i("color_tex", 2);
      prog->set_uniform2f("rand_offset", displacement.y, 0.0f);
      prog->set_uniform1f("damp", displacement.x);
    }


    glClearColor(0.5f,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    //glAlphaFunc(GL_GREATER, 0.2f);
    //glEnable(GL_ALPHA_TEST);

    {
      VertexArrayDrawable va;

      va.set_texture(0, image_surface->get_texture());
      va.set_texture(1, displace_surface->get_texture());
      va.set_texture(2, color_surface->get_texture());

      va.set_mode(GL_TRIANGLE_FAN);

      va.color(surf::Color(1.0f, 1.0f, 1.0f));
      va.texcoord(0.0f, 0.0f);
      va.vertex(0.0f, 0.0f);

      va.color(surf::Color(1.0f, 1.0f, 1.0f));
      va.texcoord(1.0f, 0.0f);
      va.vertex(1280.0f, 0.0f);

      va.color(surf::Color(1.0f, 1.0f, 1.0f));
      va.texcoord(1.0f, 1.0f);
      va.vertex(1280.0f, 800.0f);

      va.color(surf::Color(1.0f, 1.0f, 1.0f));
      va.texcoord(0.0f, 1.0f);
      va.vertex(0.0f, 800.0f);

      va.render(window->get_gc());
    }

    window->swap_buffers();
    system.delay(30);
  }

  return 0;
}

int main(int argc, char* argv[])
{
  try
  {
    App app;
    app.run(argc, argv);
  }
  catch(std::exception& err)
  {
    std::cout << err.what() << std::endl;
    return 1;
  }

  return 0;
}

/* EOF */
