/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

/*! \mainpage Windstille Documentation
 *
 * \section intro_sec Introduction
 *
 * Windstille is a classic 2d jump'n shoot game in which the player
 * will be placed in a foreign alien world and has to find its way
 * back into safety. Windstille is currently available for GNU/Linux
 * and Windows.  Its webpage at which one might find new versions,
 * source code, artworks and more informations is located at:
 *
 * http://windstille.berlios.de/
 *
 * The latest development version can be obtained by:
 * 
 * $ svn co svn://svn.berlios.de/windstille/trunk/windstille
 * 
 * Windstille is covered under the GNU GPL, which means that you can copy
 * and even modify it pretty much as you like, as long as you keep the
 * copyright headers in place and distribute the source too if you
 * distribute binaries, see the file COPYING for details.
 *
 * \section install_sec Installation
 *
 * To compile Windstille you need:
 *
 * OpenAL, OpenGL, SDL, SDL_image, freetype, libpng, physfs, boost, scons, bison, flex
 */

#ifndef HEADER_WINDSTILLE_APP_WINDSTILLE_MAIN_HPP
#define HEADER_WINDSTILLE_APP_WINDSTILLE_MAIN_HPP

#include "screen/screen.hpp"

class WindstilleMain
{  
public:
  WindstilleMain();
  ~WindstilleMain();

  int main(int argc, char** argv);

private:
  void init_sdl();
  void init_physfs(const char* argv0);
  void init_modules();
  void deinit_modules();
};

#endif

/* EOF */
