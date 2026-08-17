/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WSTDISPLAY_SYSTEM_HPP
#define HEADER_WSTDISPLAY_SYSTEM_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <sigc++/signal.h>
#include <geom/rect.hpp>
#include <geom/size.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/opengl_window.hpp>

namespace wstsys {

class System
{
public:
  System();
  ~System();

  std::unique_ptr<wstdisplay::OpenGLWindow> create_window(wstdisplay::OpenGLWindow::Params const& params);
  std::unique_ptr<wstdisplay::OpenGLWindow> create_window(std::string title, geom::isize const& size);

  void run();
  void update();
  int get_ticks();
  void delay(int msec);
  void quit();

  sigc::signal<void(SDL_Event const&)>& sig_event() { return m_sig_event; }

public:
  sigc::signal<void(SDL_KeyboardEvent const&)> sig_keyboard_event;
  sigc::signal<void(SDL_TextInputEvent const&)> sig_text_input_event;
  sigc::signal<void(SDL_TextEditingEvent const&)> sig_text_editing_event;
  sigc::signal<void(SDL_MouseButtonEvent const&)> sig_mouse_button_event;
  sigc::signal<void(SDL_MouseMotionEvent const&)> sig_mouse_motion_event;
  sigc::signal<void(SDL_MouseWheelEvent const&)> sig_mouse_wheel_event;

private:
  bool m_quit;
  std::unordered_map<uint32_t, wstdisplay::OpenGLWindow*> m_windows;
  sigc::signal<void(SDL_Event const&)> m_sig_event;

private:
  System(const System&) = delete;
  System& operator=(const System&) = delete;
};

} // namespace wstsys

#endif

/* EOF */
