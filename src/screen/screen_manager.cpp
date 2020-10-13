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

#include <iostream>
#include <fmt/format.h>

#include <wstinput/input_manager.hpp>

#include "app/app.hpp"
#include "app/config.hpp"
#include "app/sound_manager.hpp"
#include "display/display.hpp"
#include "display/opengl_window.hpp"
#include "font/fonts.hpp"
#include "screen/game_session.hpp"
#include "screen/input_configurator.hpp"
#include "util/pathname.hpp"

ScreenManager::ScreenManager() :
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
  show_controller_help_window(false),
  controller_help_window()
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

  controller_help_window.reset(new ControllerHelpWindow());
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
      g_app.input().update(delta);

      Console::current()->update(step);
      if (!Console::current()->is_active())
      {
        if (!overlay_screens.empty())
          overlay_screens.back()->update(step, g_app.input().get_controller());
        else if (!screens.empty())
          screens.back()->update(step, g_app.input().get_controller());
      }
      g_app.input().clear();

      delta -= step;
    }

    overlap_delta = delta;

    g_app.sound().update(delta);

    draw();

    frame_counter += 1;

    poll_events();

    apply_pending_actions();

    SDL_Delay(5);
  }
}

void
ScreenManager::draw()
{
  if (!screens.empty())
    screens.back()->draw();

  if (!overlay_screens.empty())
    overlay_screens.back()->draw();

  if (show_controller_help_window)
    controller_help_window->draw();

  Console::current()->draw();

  if (config.get_bool("show-fps"))
    draw_fps();

  g_app.window().swap_buffers();
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
        std::cout << "Error: ScreenManager: trying to pop_overlay with empty stack" << std::endl;
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
        std::cout << "Ctrl-c or Window-close pressed, game is going to quit" << std::endl;
        quit();
        break;

      case SDL_KEYDOWN:
      case SDL_KEYUP:
        if (event.key.state)
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_F6:
              SDL_SetRelativeMouseMode(SDL_FALSE);
              break;

            case SDLK_F7:
              SDL_SetRelativeMouseMode(SDL_TRUE);
              break;

            case SDLK_F8:
              show_controller_help_window = !show_controller_help_window;
              break;

            case SDLK_F9:
              push_overlay(new InputConfigurator());
              break;

            case SDLK_F10:
              config.set_bool("show-fps", !config.get_bool("show-fps"));
              break;

            case SDLK_F11:
              config.set_bool("fullscreen", !config.get_bool("fullscreen"));
              g_app.window().set_fullscreen(config.get_bool("fullscreen"));
              break;

            case SDLK_F12:
            {
              // FIXME: Replace this with Physfs stuff
              int count = 0;
              Pathname filename;
              do {
                filename = Pathname(fmt::format("screenshots/windstille{:04d}.png", count), Pathname::kUserPath);
                count += 1;
              } while(filename.exists());

              Display::save_screenshot(filename);
              ConsoleLog << "Writing screenshot to: '" << filename << "'" << std::endl;
            }
            break;

            default:
              if (!Console::current()->is_active())
              {
                if (!overlay_screens.empty())
                  overlay_screens.back()->handle_event(event);
                else if (!screens.empty())
                  screens.back()->handle_event(event);
              }
              break;
          }
        }

        if (!Console::current()->is_active() && event.key.state && event.key.keysym.sym == SDLK_F1)
        {
          Console::current()->activate();
        }
        else
        {
          g_app.input().on_event(event);
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
        g_app.input().on_event(event);

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

void
ScreenManager::draw_fps()
{
  if(time_counter > 1)
  {
    last_fps = int(static_cast<float>(frame_counter) / time_counter);

    time_counter  = fmodf(time_counter, 1.0f);
    frame_counter = 0;
  }

  std::ostringstream out;
  out << "FPS: " << last_fps;
  g_app.fonts().ttffont->draw(glm::vec2(static_cast<float>(Display::get_width()) - 100.0f, 30.0f), out.str());
}

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
ScreenManager::show_controller_debug(bool v)
{
  show_controller_help_window = v;
}

bool
ScreenManager::get_show_controller_debug() const
{
  return show_controller_help_window;
}

/* EOF */
