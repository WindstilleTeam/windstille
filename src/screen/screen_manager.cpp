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

#include "screen/screen_manager.hpp"

#include <logmich/log.hpp>
#include <fmt/format.h>

#include <surf/save.hpp>
#include <wstinput/input_manager.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstgui/screen.hpp>
#include <wstsound/sound_manager.hpp>

ScreenManager::ScreenManager(wstdisplay::OpenGLWindow& window, wstinput::InputManagerSDL& input, wstsound::SoundManager& sound) :
  m_window(window),
  m_input(input),
  m_sound(sound),
  screens(),
  screen_action(NONE),
  screen_screen(),
  overlay_screens(),
  overlay_screen_action(NONE),
  overlay_screen_screen(),
  ticks(0),
  time_counter(0),
  frame_counter(0),
  last_fps(0),
  overlap_delta(0),
  do_quit(false),
  m_key_bindings(),
  m_huds()
{
}

ScreenManager::~ScreenManager()
{
  screens.clear();
  overlay_screens.clear();
}

void
ScreenManager::run()
{
  do_quit = false;

  ticks = SDL_GetTicks();

  apply_pending_actions();

  while (!do_quit && !screens.empty())
  {
    /// Amount of time the world moves forward each update(), this is
    /// independed of the number of frames and always constant
    static const float step = 0.001f;

    Uint32 now = SDL_GetTicks();
    float delta = static_cast<float>(now - ticks) / 1000.0f + overlap_delta;
    ticks = now;

    time_counter += delta;

    while (delta > step)
    {
      m_input.update(delta);

      if (!overlay_screens.empty()) {
        overlay_screens.back()->update(step, m_input.get_controller());
      } else if (!screens.empty()) {
        screens.back()->update(step, m_input.get_controller());
      }

      m_input.clear();

      for(Screen* hud : m_huds) {
        hud->update(step, Controller());
      }

      delta -= step;
    }

    overlap_delta = delta;

    m_sound.update(delta);

    draw(m_window.get_gc());

    frame_counter += 1;

    poll_events();

    apply_pending_actions();

    SDL_Delay(5);
  }
}

void
ScreenManager::draw(wstdisplay::GraphicsContext& gc)
{
  if (!screens.empty()) {
    screens.back()->draw(gc);
  }

  if (!overlay_screens.empty()) {
    overlay_screens.back()->draw(gc);
  }

  for(Screen* hud : m_huds) {
    hud->draw(gc);
  }

#if FIXME
  if (config.get_bool("show-fps")) {
    draw_fps(gc);
  }
#endif
  m_window.swap_buffers();
}

void
ScreenManager::apply_pending_actions()
{
  // Commit any pending screen actions
  switch(overlay_screen_action)
  {
    case PUSH_SCREEN:
      overlay_screens.push_back(overlay_screen_screen);
      overlay_screen_screen = std::shared_ptr<Screen>();
      break;

    case POP_SCREEN:
      if (overlay_screens.empty())
      {
        log_error("ScreenManager: trying to pop_overlay with empty stack");
      }
      else
      {
        overlay_screens.pop_back();
      }
      break;

    case CLEAR_SCREENS:
      overlay_screens.clear();
      break;

    case NONE:
      // nothing
      break;
  }
  overlay_screen_action = NONE;

  switch(screen_action)
  {
    case PUSH_SCREEN:
      screens.push_back(screen_screen);
      screen_screen = std::shared_ptr<Screen>();
      screens.back()->on_startup();
      break;

    case POP_SCREEN:
      screens.pop_back();
      if (!screens.empty())
        screens.back()->on_startup();
      break;

    case CLEAR_SCREENS:
      screens.clear();
      break;

    case NONE:
      // nothing
      break;
  }
  screen_action = NONE;
}

void
ScreenManager::poll_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
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
            if (!overlay_screens.empty()) {
              overlay_screens.back()->handle_event(event);
            } else if (!screens.empty()) {
              screens.back()->handle_event(event);
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
        m_input.on_event(event);

        if (!overlay_screens.empty())
          overlay_screens.back()->handle_event(event);
        break;

      default:
        if (!overlay_screens.empty())
          overlay_screens.back()->handle_event(event);
        else if (!screens.empty())
          screens.back()->handle_event(event);
        break;
    }
  }
}

#ifdef FIXME
void
ScreenManager::draw_fps(wstdisplay::GraphicsContext& gc)
{
  if(time_counter > 1)
  {
    last_fps = int(static_cast<float>(frame_counter) / time_counter);

    time_counter  = fmodf(time_counter, 1.0f);
    frame_counter = 0;
  }

  std::ostringstream out;
  out << "FPS: " << last_fps;
  g_app.fonts().ttffont->draw(gc, glm::vec2(static_cast<float>(gc.size().width()) - 100.0f, 30.0f), out.str());
}
#endif

void
ScreenManager::push_screen(Screen* s)
{
  assert(screen_screen == nullptr);

  screen_action = PUSH_SCREEN;
  screen_screen = std::shared_ptr<Screen>(s);
}

void
ScreenManager::pop_screen()
{
  screen_action = POP_SCREEN;
}

void
ScreenManager::push_overlay(Screen* s)
{
  assert(!overlay_screen_screen.get());

  overlay_screen_action = PUSH_SCREEN;
  overlay_screen_screen = std::shared_ptr<Screen>(s);
}

void
ScreenManager::pop_overlay()
{
  overlay_screen_action = POP_SCREEN;
}

void
ScreenManager::clear_overlay()
{
  overlay_screen_action = CLEAR_SCREENS;
}

void
ScreenManager::quit()
{
  do_quit = true;
}

void
ScreenManager::bind_key(SDL_KeyCode code, std::function<void()> callback)
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

/* EOF */
