/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include <memory>
#include <memory>
#include <vector>

#include "util/currenton.hpp"

class Screen;
class ControllerHelpWindow;

/**
 *  The ScreenManager handles overlays like Option Menus, Main Menus
 *  and such
 */
class ScreenManager : public Currenton<ScreenManager>
{
private:
  enum ScreenAction { NONE, POP_SCREEN, PUSH_SCREEN, CLEAR_SCREENS };

  typedef std::vector<std::shared_ptr<Screen> > Screens;
  Screens screens;
  ScreenAction screen_action;
  std::shared_ptr<Screen> screen_screen;

  Screens overlay_screens;
  ScreenAction overlay_screen_action;
  std::shared_ptr<Screen> overlay_screen_screen;

  unsigned int ticks;

  float time_counter;
  int   frame_counter;
  int   last_fps;
  float overlap_delta;
  bool  do_quit;
  bool  show_controller_help_window;
  std::unique_ptr<ControllerHelpWindow> controller_help_window;

  void apply_pending_actions();
  void draw();

public:
  ScreenManager();
  ~ScreenManager() override;

  /** Displays the previously set screen in until quit() is called */
  void run();

  /** Breaks out of the run() function */
  void quit();

  /** Push a screen, the screen will be delete'ed once it is no longer needed */
  void push_screen(Screen* s);
  void pop_screen();

  /**
   *  Push an overlay screen, the screen will be delete'ed once it is
   *  no longer needed. An overlay is a non-fullscreen screen like a
   *  menu that is layed over a fullscreen-screen and receives input.
   */
  void push_overlay(Screen* s);
  void pop_overlay();
  void clear_overlay();

  // Callbacks, FIXME: Could be moved to a seperate class
  void show_controller_debug(bool v);
  bool get_show_controller_debug() const;

private:
  void poll_events();
  void draw_fps();

  ScreenManager (const ScreenManager&);
  ScreenManager& operator= (const ScreenManager&);
};

#endif

/* EOF */
