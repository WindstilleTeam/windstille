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

#include "slideshow/slideshow.hpp"

#include <SDL.h>

#include <sstream>
#include <stdexcept>

#include "util/command_line.hpp"
#include "display/assert_gl.hpp"
#include "display/opengl_window.hpp"
#include "display/framebuffer.hpp"
#include "display/surface_manager.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/surface.hpp"
#include "display/display.hpp"
#include "display/texture_manager.hpp"
#include "util/system.hpp"

#include "slideshow/slide_show.hpp"
#include "slideshow/slide_builder.hpp"

App::App() :
  m_aspect_ratio(768, 576),
  m_window_size(768, 576),
  m_fullscreen(false),
  m_files(),
  m_output_dir(),
  m_fps(25.0f),
  m_edit_mode(false),
  m_start_time(0.0f),
  m_start_frame(-1)
{
}

void
App::init_sdl()
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
}

void
App::parse_args(int argc, char** argv)
{
  CommandLine argp;
  argp.add_usage("[OPTIONS] FILE...");
  argp.add_doc("A script driven slideshow viewer");

  argp.add_group("Options:");
  argp.add_option('f', "fullscreen", "", "Use fullscreen mode");
  argp.add_option('e', "edit", "", "Edit mode");
  argp.add_option('g', "geometry", "WxH", "Use given geometry");
  argp.add_option('a', "aspect", "WxH", "Use given aspect ratio");
  argp.add_option('b', "breakpoint", "POINT", "Start at POINT");
  argp.add_option('F', "fps", "FPS", "Generate FPS frames per seconds");
  argp.add_option('o', "output", "DIR", "Write screenshots to DIR");
  argp.add_option('s', "start", "SEC", "Time where the playback should start");
  argp.add_option('S', "frame", "FRAME", "Frame at which offline rendering should start");
  argp.add_option('h', "help", "", "Print help");

  argp.parse_args(argc, argv);

  while (argp.next())
  {
    switch (argp.get_key())
    {
      case 'f':
        m_fullscreen = true;
        break;

      case 'S':
        m_start_frame = std::stoi(argp.get_argument());
        break;

      case 'e':
        m_edit_mode = true;
        break;

      case 'F':
        m_fps = std::stof(argp.get_argument());
        break;

      case 'g':
        if (sscanf(argp.get_argument().c_str(), "%dx%d", &m_window_size.width, &m_window_size.height) != 2)
        {
          throw std::runtime_error("--geometry argument wrong");
        }
        break;

      case 'a':
        if (sscanf(argp.get_argument().c_str(), "%dx%d", &m_aspect_ratio.width, &m_aspect_ratio.height) != 2)
        {
          throw std::runtime_error("--aspect argument wrong");
        }
        break;

      case 'b':
        break;

      case 's':
        m_start_time = std::stof(argp.get_argument());
        break;

      case 'o':
        m_output_dir = argp.get_argument();
        break;

      case 'h':
        argp.print_help();
        exit(0);
        break;

      case CommandLine::REST_ARG:
        m_files.push_back(argp.get_argument());
        break;

      default:
        throw std::runtime_error("unhandled argument: " + std::string(1, argp.get_key()));
    }
  }

  if (m_files.empty())
  {
    argp.print_help();
    exit(0);
  }
}

int
App::run(int argc, char* argv[])
{
  parse_args(argc, argv);

  init_sdl();

  //std::cout << "OpenGLWindow" << std::endl;
  OpenGLWindow window("Slideshow",
                      m_window_size, // window size
                      m_aspect_ratio, // aspect ratio
                      m_fullscreen, // fullscreen
                      4); // anti-alias

  TextureManager    texture_manager;
  SurfaceManager    surface_manager;

  SlideShow slide_show;

  for(std::vector<std::string>::iterator i = m_files.begin(); i != m_files.end(); ++i)
  {
    slide_show.load(*i, m_aspect_ratio);
  }

  FramebufferPtr framebuffer_multisample = Framebuffer::create(m_window_size.width, m_window_size.height, 8);
  FramebufferPtr framebuffer = Framebuffer::create(m_window_size.width, m_window_size.height);

  bool loop = true;
  bool pause = false;

  int last_percent_complete = -1;

  int frame_number = 0;
  Uint32 last_ticks = SDL_GetTicks();
  float time = m_start_time;
  bool fast_forward = false;

  if (m_start_frame != -1)
  {
    frame_number = m_start_frame;
    time = static_cast<float>(frame_number) / m_fps;
  }

  while(loop && (!slide_show.done(time) || m_edit_mode))
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
          if (event.key.state && m_output_dir.empty()) // ignore keypresses for offline rendereing
          {
            switch (event.key.keysym.sym)
            {
              case SDLK_ESCAPE:
                loop = false;
                break;

              case SDLK_F5:
                slide_show.clear();
                for(size_t i = 0; i < m_files.size(); ++i)
                {
                  slide_show.load(m_files[i], m_aspect_ratio);
                }
                break;

              case SDLK_F10:
                Display::save_screenshot(Pathname("/tmp/out.png", Pathname::kSysPath));
                break;

              case SDLK_LEFT:
                time -= 1.0f;
                break;

              case SDLK_RIGHT:
                time += 1.0f;
                break;

              case SDLK_HOME:
                time = 0.0f;
                break;

              case SDLK_END:
                time = slide_show.length();
                break;

              case SDLK_PAGEUP:
                time = slide_show.find_prev(time);
                break;

              case SDLK_PAGEDOWN:
                time = slide_show.find_next(time);
                break;

              case SDLK_e:
                m_edit_mode = !m_edit_mode;
                break;

              case SDLK_f:
                fast_forward = !fast_forward;
                std::cout << fast_forward << std::endl;
                break;

              case SDLK_UP:
                time += 10.0f;
                break;

              case SDLK_DOWN:
                time -= 10.0f;
                break;

              case SDLK_SPACE:
                pause = !pause;
                break;

              default:
                break;
            }
          }
      }
    }

    if (m_output_dir.empty())
    {
      // realtime rendering
      Uint32 ticks = SDL_GetTicks();

      if (!pause)
      {
        if (fast_forward)
        {
          time += (static_cast<float>(ticks - last_ticks) / 1000.0f) * 25.0f;
        }
        else
        {
          time += static_cast<float>(ticks - last_ticks) / 1000.0f;
        }
      }
      last_ticks = ticks;

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      slide_show.draw(time, m_edit_mode);

      window.swap_buffers();

      SDL_Delay(30);
    }
    else
    {
      time += 1.0f/m_fps;

      // rendering to output dir
      Display::push_framebuffer(framebuffer_multisample);
      assert_gl("aeuthnoethuth");
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      assert_gl("clear");
      slide_show.draw(time, m_edit_mode);
      assert_gl("draw");
      //SDL_GL_SwapBuffers();
      Display::pop_framebuffer();

      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer_multisample->get_handle());
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer->get_handle());

      glBlitFramebufferEXT(0, 0, framebuffer_multisample->get_width(), framebuffer_multisample->get_height(),
                           0, 0, framebuffer->get_width(), framebuffer->get_height(),
                           GL_COLOR_BUFFER_BIT, GL_LINEAR /*NEAREST*/);

      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

      Display::push_framebuffer(framebuffer);
      char out[1024];
      sprintf(out, "%s/%08d.jpg", m_output_dir.c_str(), frame_number);
      Display::save_screenshot(Pathname(out, Pathname::kSysPath));
      //std::cout << "Wrote: " << out << std::endl;
      frame_number += 1;
      Display::pop_framebuffer();

      int percent = static_cast<int>(time / slide_show.length() * 100.0f);
      if (percent != last_percent_complete)
      {
        std::cout << "Completed: " << percent << "% - " << frame_number << std::endl;
        last_percent_complete = percent;
      }
    }

    if (m_edit_mode)
    {
      std::cout << "Time: " << time << std::endl;
    }
  }


  if (!loop)
    std::cout << "Playback interrupted: " << std::endl;
  else
    std::cout << "Playback finished: " << std::endl;

  std::cout << "  frame: " << frame_number << std::endl;
  std::cout << "  time:  " << time << std::endl;

  return 0;
}

int main(int argc, char** argv)
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
