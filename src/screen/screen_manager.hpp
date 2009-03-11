/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_SCREEN_SCREEN_MANAGER_HPP
#define HEADER_WINDSTILLE_SCREEN_SCREEN_MANAGER_HPP

#include <boost/shared_ptr.hpp>

class Screen;

/**
 *  The ScreenManager handles overlays like Option Menus, Main Menus
 *  and such
 */
class ScreenManager
{
private:
  enum ScreenAction { NONE, POP_SCREEN, PUSH_SCREEN, CLEAR_SCREENS };

  typedef std::vector<boost::shared_ptr<Screen> > Screens;
  Screens screens;
  ScreenAction screen_action;
  boost::shared_ptr<Screen> screen_screen;

  Screens overlay_screens;
  ScreenAction overlay_screen_action;
  boost::shared_ptr<Screen> overlay_screen_screen;

  unsigned int ticks;

  float time_counter;
  int   frame_counter;
  int   last_fps;
  float overlap_delta;
  bool  do_quit;
  bool show_controller_help_window;

public:
  ScreenManager();
  ~ScreenManager();

  /** Displays the previously set screen in until quit() is called */
  void run();

  /** Breaks out of the run() function */
  void quit();

  /** Push a screen, the screen will be delete'ed once it is no longer needed */
  void push_screen(Screen* s);
  void pop_screen();

  /** Push an overlay screen, the screen will be delete'ed once it is no longer needed */
  void push_overlay(Screen* s);
  void pop_overlay();
  void clear_overlay();

  // Callbacks, FIXME: Could be moved to a seperate class
  void show_fps(int i);

private:
  void poll_events();
  void draw_fps();

  ScreenManager (const ScreenManager&);
  ScreenManager& operator= (const ScreenManager&);
};

extern ScreenManager screen_manager; 

#endif

/* EOF */
