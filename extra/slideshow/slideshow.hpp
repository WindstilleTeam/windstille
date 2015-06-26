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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDESHOW_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDESHOW_HPP

#include <vector>

#include "math/size.hpp"

class App
{
private:
  Size m_aspect_ratio;
  Size m_window_size;
  bool m_fullscreen;
  std::vector<std::string> m_files;
  std::string m_output_dir;
  float m_fps;
  bool m_edit_mode;
  float m_start_time;
  int m_start_frame;

public:
  App();

  void init_sdl();
  int run(int argc, char* argv[]);
  void parse_args(int argc, char** argv);

private:
  App(const App&);
  App& operator=(const App&);
};

#endif

/* EOF */
