/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/opengl_window.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL_image.h>

#include "util/util.hpp" 
#include "util/pathname.hpp"
#include "display/opengl_state.hpp"
#include "display/display.hpp"
#include "app/config.hpp"

OpenGLWindow::OpenGLWindow()
{
  atexit(SDL_Quit);

  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  5);

  if (config.get_int("anti-aliasing"))
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ); // boolean value, either it's enabled or not
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, config.get_int("anti-aliasing") ); // 0, 2, or 4 for number of samples
    }
  
  SDL_WM_SetCaption("Windstille", "Windstille");
  SDL_WM_SetIcon(IMG_Load(Pathname("icon.png").get_sys_path().c_str()), NULL);

  m_window = SDL_SetVideoMode(config.get_int("screen-width"), config.get_int("screen-height"),
                              0, SDL_OPENGL | (config.get_bool("fullscreen") ? SDL_FULLSCREEN : 0));

  if (!m_window)
    {
      throw std::runtime_error("Display:: Couldn't create window");
    }
  else
    {
      GLenum err = glewInit();
      if(err != GLEW_OK) {
        std::ostringstream msg;
        msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
        throw std::runtime_error(msg.str());
      }
      /*
        if(!GLEW_EXT_framebuffer_object) {
        std::ostringstream msg;
        msg << "Display:: Framebuffer opengl extension not supported";
        throw std::runtime_error(msg.str());
        }
      */

      glViewport(0, 0, m_window->w, m_window->h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      static const float cl_pixelcenter_constant = 0.375;

      //glOrtho(0.0, window->w, window->h, 0.0, -1000.0, 1000.0);
  
      // glOrtho(0.0, 800, 0.0, 600.0, 1000.0, -1000.0); // proper right-hand CO
      Display::aspect_size = Size(config.get_int("aspect-width"), 
                                  config.get_int("aspect-height"));

      glOrtho(0.0, 
              Display::get_width(), Display::get_height(),
              0.0, 1000.0, -1000.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslated(cl_pixelcenter_constant, cl_pixelcenter_constant, 0.0);

      if (config.get_int("anti-aliasing"))
        glEnable(GL_MULTISAMPLE_ARB); 

      assert_gl("setup projection");

      OpenGLState::init();
    }
}

OpenGLWindow::~OpenGLWindow()
{
  // must not free m_window, see:
  // http://sdl.beuc.net/sdl.wiki/SDL_SetVideoMode
} 

void
OpenGLWindow::set_fullscreen(bool fullscreen)
{
  Uint32 flags = SDL_OPENGL;

  if (fullscreen)
    flags |= SDL_FULLSCREEN;

  m_window = SDL_SetVideoMode(config.get_int("screen-width"), config.get_int("screen-height"), 0, flags);

  if (!m_window)
    {
      throw std::runtime_error("OpenGLWindow: Couldn't create window");
    }
}

/* EOF */
