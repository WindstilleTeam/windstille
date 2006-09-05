/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_SCREEN_MANAGER_HPP
#define HEADER_SCREEN_MANAGER_HPP

class Screen;

/** */
class ScreenManager
{
private:
  Screen* screen;
  Screen* overlay_screen;

  Screen* next_overlay_screen;
  bool    has_next_overlay_screen;

  unsigned int ticks;

  float time_counter;
  int   frame_counter;
  int   last_fps;
  float overlap_delta;
  bool  do_quit;

public:
  ScreenManager();
  ~ScreenManager();

  /** Displays the previously set screen in until quit() is called */
  void run();

  /** Breaks out of the run() function */
  void quit();

  /** Sets the currently active screen */
  void set_screen(Screen* s);

  /** Sets the overlay, which is a screen drawn on-top of the current
      screen, usefull for menus or console, use set_overlay(0) to kill
      the current overlay */
  void set_overlay(Screen* s);

private:
  void poll_events();
  void draw_fps();

  ScreenManager (const ScreenManager&);
  ScreenManager& operator= (const ScreenManager&);
};

extern ScreenManager screen_manager; 

#endif

/* EOF */
