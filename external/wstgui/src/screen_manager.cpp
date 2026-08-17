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

#include "screen_manager.hpp"

#include <logmich/log.hpp>
#include <fmt/format.h>

#include <surf/save.hpp>
#include <wstinput/input_manager.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstgui/screen.hpp>
#include <wstsound/sound_manager.hpp>

namespace wstgui {

ScreenManager::ScreenManager(wstsys::System& system, wstdisplay::OpenGLWindow& window, wstinput::InputManagerSDL& input) :
  m_system(system),
  m_window(window),
  m_input(input),
  m_screens(),
  m_screen_action(ScreenAction::NONE),
  m_screen_screen(),
  m_overlay_screens(),
  m_overlay_screen_action(ScreenAction::NONE),
  m_overlay_screen_screen(),
  m_ticks(0),
  m_overlap_delta(0),
  m_do_quit(false),
  m_key_bindings(),
  m_huds(),
  m_sig_update(),
  m_sig_draw_begin(),
  m_sig_draw_end()
{
  m_system.sig_event().connect([this](SDL_Event const& ev){ handle_event(ev); });
}

ScreenManager::~ScreenManager()
{
  m_screens.clear();
  m_overlay_screens.clear();
}

void
ScreenManager::run()
{
  m_do_quit = false;

  m_ticks = m_system.get_ticks();

  apply_pending_actions();

  while (!m_do_quit && !m_screens.empty())
  {
    /// Amount of time the world moves forward each update(), this is
    /// independed of the number of frames and always constant
    static const float step = 0.001f;

    Uint32 const now = m_system.get_ticks();
    float delta = static_cast<float>(now - m_ticks) / 1000.0f + m_overlap_delta;
    m_ticks = now;

    while (delta > step)
    {
      m_input.update(delta);

      if (!m_overlay_screens.empty()) {
        m_overlay_screens.back()->update(step, m_input.get_controller());
      } else if (!m_screens.empty()) {
        m_screens.back()->update(step, m_input.get_controller());
      }

      for(Screen* hud : m_huds) {
        hud->update(step, m_input.get_controller());
      }
      m_input.clear();

      delta -= step;
    }

    m_overlap_delta = delta;

    m_sig_update(delta);
    m_system.update();

    draw(m_window.get_gc());

    apply_pending_actions();

    m_system.delay(5);
  }
}

void
ScreenManager::draw(wstdisplay::GraphicsContext& gc)
{
  m_sig_draw_begin(gc);

  if (!m_screens.empty()) {
    m_screens.back()->draw(gc);
  }

  if (!m_overlay_screens.empty()) {
    m_overlay_screens.back()->draw(gc);
  }

  for(Screen* hud : m_huds) {
    hud->draw(gc);
  }

  m_sig_draw_end(gc);

  m_window.swap_buffers();
}

void
ScreenManager::apply_pending_actions()
{
  // Commit any pending screen actions
  switch (m_overlay_screen_action)
  {
    case ScreenAction::PUSH_SCREEN:
      m_overlay_screens.push_back(m_overlay_screen_screen);
      m_overlay_screen_screen = std::shared_ptr<Screen>();
      break;

    case ScreenAction::POP_SCREEN:
      if (m_overlay_screens.empty())
      {
        log_error("ScreenManager: trying to pop_overlay with empty stack");
      }
      else
      {
        m_overlay_screens.pop_back();
      }
      break;

    case ScreenAction::CLEAR_SCREENS:
      m_overlay_screens.clear();
      break;

    case ScreenAction::NONE:
      // nothing
      break;
  }

  m_overlay_screen_action = ScreenAction::NONE;

  switch (m_screen_action)
  {
    case ScreenAction::PUSH_SCREEN:
      m_screens.push_back(m_screen_screen);
      m_screen_screen = std::shared_ptr<Screen>();
      m_screens.back()->on_startup();
      break;

    case ScreenAction::POP_SCREEN:
      m_screens.pop_back();
      if (!m_screens.empty())
        m_screens.back()->on_startup();
      break;

    case ScreenAction::CLEAR_SCREENS:
      m_screens.clear();
      break;

    case ScreenAction::NONE:
      // nothing
      break;
  }
  m_screen_action = ScreenAction::NONE;

  while (!m_overlay_screens.empty() &&
         m_overlay_screens.back()->is_finished()) {
    m_overlay_screens.pop_back();
  }

  while (!m_screens.empty() &&
         m_screens.back()->is_finished()) {
    m_screens.pop_back();
  }
}

void
ScreenManager::handle_event(const SDL_Event& event)
{
  switch(event.type)
  {
    case SDL_QUIT:
      // FIXME: This should be a bit more gentle, but will do for now
      log_info("Ctrl-c or Window-close pressed, game is going to quit");
      quit();
      break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (event.key.state) {
        if (auto binding = m_key_bindings.find(event.key.keysym.sym); binding != m_key_bindings.end()) {
          binding->second();
        } else {
          if (!m_overlay_screens.empty()) {
            m_overlay_screens.back()->handle_event(event);
          } else if (!m_screens.empty()) {
            m_screens.back()->handle_event(event);
          }
        }
      }
      break;

    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEMOTION:
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
    case SDL_TEXTINPUT:
    case SDL_TEXTEDITING:
      if (!m_overlay_screens.empty()) {
        m_overlay_screens.back()->handle_event(event);
      }
      break;

    default:
      if (!m_overlay_screens.empty()) {
        m_overlay_screens.back()->handle_event(event);
      } else if (!m_screens.empty()) {
        m_screens.back()->handle_event(event);
      }
      break;
  }

  m_input.on_event(event);
}

void
ScreenManager::push_screen(std::unique_ptr<Screen> s)
{
  assert(m_screen_screen == nullptr);

  m_screen_action = ScreenAction::PUSH_SCREEN;
  m_screen_screen = std::shared_ptr<Screen>(std::move(s));
}

void
ScreenManager::pop_screen()
{
  m_screen_action = ScreenAction::POP_SCREEN;
}

void
ScreenManager::push_overlay(std::unique_ptr<Screen> s)
{
  assert(!m_overlay_screen_screen.get());

  m_overlay_screen_action = ScreenAction::PUSH_SCREEN;
  m_overlay_screen_screen = std::shared_ptr<Screen>(std::move(s));
}

void
ScreenManager::pop_overlay()
{
  m_overlay_screen_action = ScreenAction::POP_SCREEN;
}

void
ScreenManager::clear_overlay()
{
  m_overlay_screen_action = ScreenAction::CLEAR_SCREENS;
}

void
ScreenManager::quit()
{
  m_do_quit = true;
}

void
ScreenManager::bind_key(SDL_Keycode code, std::function<void()> callback)
{
  m_key_bindings[code] = std::move(callback);
}

void
ScreenManager::add_hud(Screen* screen)
{
  m_huds.emplace_back(screen);
}

void
ScreenManager::remove_hud(Screen* screen)
{
  std::erase(m_huds, screen);
}

} // namespace wstgui

/* EOF */
