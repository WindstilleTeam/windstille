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

#include <SDL.h>

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/device.hpp>
#include <wstdisplay/material.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/renderer.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/texture_params.hpp>
#include <wstsystem/system.hpp>

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

  wstsystem::System system;
  auto window = system.create_window("Shader Test", m_window_size);
  Device& device = window->get_device();
  Renderer& renderer = window->get_renderer();
  Canvas canvas;

  TextureManager texture_manager(device);
  TextureParams const repeat{
    .filter = TextureFilter::Linear,
    .wrap_x = TextureWrap::Repeat,
    .wrap_y = TextureWrap::Repeat
  };
  TextureId const image_texture = texture_manager.get(argv[1], repeat).get_texture();
  TextureId const displace_texture = texture_manager.get(argv[2], repeat).get_texture();
  TextureId const color_texture = texture_manager.get(argv[3], repeat).get_texture();

  Unique<ShaderProgram> program = device.load_program(argv[4], argv[5]);
  Unique<Material> material = device.create_material(program);
  material->set_texture(1, displace_texture);
  material->set_texture(2, color_texture);

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
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            loop = false;
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

    material->set_uniform("rand_offset", glm::vec2(displacement.y, 0.0f));
    material->set_uniform("damp", displacement.x);

    Vertex const quad[] = {
      {0.0f, 0.0f, 0.0f, 0.0f},
      {1280.0f, 0.0f, 1.0f, 0.0f},
      {1280.0f, 800.0f, 1.0f, 1.0f},
      {0.0f, 800.0f, 0.0f, 1.0f},
    };

    canvas.clear();
    canvas.set_material(material);
    canvas.draw_quads(image_texture, quad);

    renderer.begin_frame(window->get_drawable_size());
    renderer.render(canvas, RenderPass{.clear = surf::Color(0.5f, 0.0f, 0.0f, 0.0f)});
    renderer.end_frame();

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
