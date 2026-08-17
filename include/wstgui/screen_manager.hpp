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

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <sigc++/signal.h>

#include <wstdisplay/fwd.hpp>
#include <wstgui/fwd.hpp>
#include <wstsystem/system.hpp>
#include <wstinput/fwd.hpp>

namespace wstgui {

/**
 *  The ScreenManager handles overlays like Option Menus, Main Menus
 *  and such
 */
class ScreenManager
{
public:
  ScreenManager(wstsys::System& system, wstdisplay::OpenGLWindow& window, wstinput::InputManagerSDL& input);
  ~ScreenManager();

  /** Displays the previously set screen in until quit() is called */
  void run();

  /** Breaks out of the run() function */
  void quit();

  /** Push a screen, the screen will be delete'ed once it is no longer needed */
  void push_screen(std::unique_ptr<Screen> s);
  void pop_screen();

  /**
   *  Push an overlay screen, the screen will be delete'ed once it is
   *  no longer needed. An overlay is a non-fullscreen screen like a
   *  menu that is layed over a fullscreen-screen and receives input.
   */
  void push_overlay(std::unique_ptr<Screen> s);
  void pop_overlay();
  void clear_overlay();

  /** Add a non-interactive overlay */
  void add_hud(Screen* screen);
  void remove_hud(Screen* screen);

  void bind_key(SDL_Keycode code, std::function<void()> callback);

  sigc::signal<void (float)>& sig_update() { return m_sig_update; };
  sigc::signal<void (wstdisplay::GraphicsContext&)>& sig_draw_begin() { return m_sig_draw_begin; };
  sigc::signal<void (wstdisplay::GraphicsContext&)>& sig_draw_end() { return m_sig_draw_end; };

private:
  void apply_pending_actions();
  void draw(wstdisplay::GraphicsContext& gc);
  void handle_event(const SDL_Event& event);

private:
  enum class ScreenAction { NONE, POP_SCREEN, PUSH_SCREEN, CLEAR_SCREENS };

private:
  wstsys::System& m_system;
  wstdisplay::OpenGLWindow& m_window;
  wstinput::InputManagerSDL& m_input;

  std::vector<std::shared_ptr<Screen>> m_screens;
  ScreenAction m_screen_action;
  std::shared_ptr<Screen> m_screen_screen;

  std::vector<std::shared_ptr<Screen>>  m_overlay_screens;
  ScreenAction m_overlay_screen_action;
  std::shared_ptr<Screen> m_overlay_screen_screen;

  unsigned int m_ticks;

  float m_overlap_delta;
  bool  m_do_quit;

  std::unordered_map<SDL_Keycode, std::function<void()>> m_key_bindings;
  std::vector<Screen*> m_huds;

  sigc::signal<void (float)> m_sig_update;
  sigc::signal<void (wstdisplay::GraphicsContext&)> m_sig_draw_begin;
  sigc::signal<void (wstdisplay::GraphicsContext&)> m_sig_draw_end;

public:
  ScreenManager (const ScreenManager&) = delete;
  ScreenManager& operator= (const ScreenManager&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
