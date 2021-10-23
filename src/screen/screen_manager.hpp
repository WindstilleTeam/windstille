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

#include <SDL.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wstdisplay/fwd.hpp>
#include <wstgui/fwd.hpp>
#include <wstinput/fwd.hpp>
#include <wstsound/fwd.hpp>

/**
 *  The ScreenManager handles overlays like Option Menus, Main Menus
 *  and such
 */
class ScreenManager
{
public:
  ScreenManager(wstdisplay::OpenGLWindow& window, wstinput::InputManagerSDL& input, wstsound::SoundManager& sound);
  ~ScreenManager();

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

  /** Add a non-interactive overlay */
  void add_hud(Screen* screen);
  void remove_hud(Screen* screen);

  void bind_key(SDL_KeyCode code, std::function<void()> callback);

private:
  void apply_pending_actions();
  void draw(wstdisplay::GraphicsContext& gc);
  void poll_events();

private:
  enum ScreenAction { NONE, POP_SCREEN, PUSH_SCREEN, CLEAR_SCREENS };

private:
  wstdisplay::OpenGLWindow& m_window;
  wstinput::InputManagerSDL& m_input;
  wstsound::SoundManager& m_sound;

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

  std::unordered_map<SDL_Keycode, std::function<void()>> m_key_bindings;
  std::vector<Screen*> m_huds;

public:
  ScreenManager (const ScreenManager&) = delete;
  ScreenManager& operator= (const ScreenManager&) = delete;
};

#endif

/* EOF */
