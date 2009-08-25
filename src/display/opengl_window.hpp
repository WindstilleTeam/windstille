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

#ifndef HEADER_WINDSTILLE_DISPLAY_OPENGL_WINDOW_HPP
#define HEADER_WINDSTILLE_DISPLAY_OPENGL_WINDOW_HPP

#include <SDL.h>

#include "util/currenton.hpp"

class OpenGLWindow : public Currenton<OpenGLWindow>
{
private:
  SDL_Surface* m_window;

public:
  OpenGLWindow();
  ~OpenGLWindow();

  int get_width()  const { return m_window->w; }
  int get_height() const { return m_window->h; }

  void set_fullscreen(bool fullscreen);
  void set_gamma(float r, float g, float b);

private:
  OpenGLWindow(const OpenGLWindow&);
  OpenGLWindow& operator=(const OpenGLWindow&);
};

#endif

/* EOF */
