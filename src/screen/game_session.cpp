/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "screen/game_session.hpp"

#include <geom/geom.hpp>
#include <wstgui/screen_manager.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include "app/sound_manager.hpp"
#include <wstdisplay/compositor.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include "engine/script_manager.hpp"
#include "engine/sector.hpp"
#include "font/fonts.hpp"
#include "hud/conversation.hpp"
#include "hud/dialog_manager.hpp"
#include "hud/inventory.hpp"
#include "hud/pda.hpp"
#include "hud/speech_manager.hpp"
#include "objects/doll.hpp"
#include "objects/player.hpp"
#include "screen/view.hpp"

class GameSessionImpl
{
public:
  wstdisplay::Compositor compositor;
  wstdisplay::SceneContext sc;

  float fadeout_value;
  float fade_time;
  std::unique_ptr<Sector> sector;
  View    view;

  Pathname filename;

  bool pause;

  /** Amount of how far the cutscene bars are visible, 0.0 means not
      visible, 1.0 fully visible. */
  float cutscene_value;

  /** True if cutscene is active, false otherwhise */
  bool  cutscene_mode;

  enum { NO_ACTION, QUIT_ACTION, CHANGE_SECTOR_ACTION } next_action;

  enum { FADEIN, RUNNING, FADEOUT } fade_state;

  surf::Color fade_color;

  SpeechManager speech_manager;

  // Active GUI Elements
  DialogManager dialog_manager;
  Conversation  conversation;
  Inventory     inventory;
  PDA           pda;

  /** Pointer to either pda, inventory, conversation or
      dialog_manager. Receives input and gets drawn to the screen */
  wstgui::Screen* current_gui;

  GameSessionImpl()
    : compositor(g_app.window().get_size(),
                 g_app.window().get_gc().size()),
      sc(),
      fadeout_value(),
      fade_time(),
      sector(),
      view(),
      filename(),
      pause(),
      cutscene_value(),
      cutscene_mode(),
      next_action(),
      fade_state(),
      fade_color(),
      speech_manager(),
      dialog_manager(),
      conversation(),
      inventory(),
      pda(),
      current_gui()
  {
    current_gui    = nullptr;
    cutscene_mode  = false;
    cutscene_value = 0.0f;
    fade_color     = surf::Color(0.0f, 0.0f, 0.0f, 1.0f);
    fade_state     = RUNNING;
    fadeout_value  = 0.0f;
    fade_time      = 1.0f;
  }

  ~GameSessionImpl()
  {
  }

  void draw(wstdisplay::GraphicsContext& gc);

  void update_cutscene(float delta);
  void update_input(float delta);
  void update_controller(float delta, wstinput::Controller const& controller);
  void update_game(float delta);
  void update(float delta, wstinput::Controller const& controller);

  void handle_event(SDL_Event const& event);

private:
  GameSessionImpl (GameSessionImpl const&);
  GameSessionImpl& operator= (GameSessionImpl const&);
};

GameSession::GameSession(Pathname const& arg_filename)
  : impl(new GameSessionImpl())
{
  impl->filename = arg_filename;

  impl->pause = false;

  set_sector(impl->filename);
}

GameSession::~GameSession()
{
}

void
GameSessionImpl::draw(wstdisplay::GraphicsContext& gc)
{
  view.draw(sc, *sector);

  // Render the scene to the screen
  compositor.render(gc, sc, &sector->get_scene_graph(), view.get_gc_state());

  if (cutscene_mode || cutscene_value > 0.0f)
  {
    int border_size = static_cast<int>(75 * cutscene_value);
    gc.fill_rect(geom::frect(geom::irect(geom::ipoint(0, 0), geom::isize(gc.size().width(), border_size))),
                 surf::Color(0.0f, 0.0f, 0.0f, cutscene_value));
    gc.fill_rect(geom::frect(geom::irect(geom::ipoint(0, gc.size().height() - border_size), geom::isize(gc.size().width(), border_size))),
                 surf::Color(0.0f, 0.0f, 0.0f, cutscene_value));
  }

  if (current_gui)
    current_gui->draw(gc);

  if (fade_state == FADEOUT || fade_state == FADEIN)
  {
    gc.fill_rect(geom::frect(geom::irect(0, 0,
                                         gc.size().width(), gc.size().height())),
                 surf::Color(fade_color.r, fade_color.g, fade_color.b, fadeout_value));
  }

  speech_manager.draw(gc);

  if (pause)
  {
    if ((SDL_GetTicks() / 1000) % 2)
      g_app.fonts().vera20->draw(gc, glm::vec2(static_cast<float>(gc.size().width())  / 2.0f,
                                               static_cast<float>(gc.size().height()) / 2.0f),
                                 "Pause");
  }
}

void
GameSessionImpl::update_cutscene(float delta)
{
  // Cutscene stuff (black bars that fade-in/out
  if (cutscene_mode)
    cutscene_value += delta * 0.75f;
  else
    cutscene_value -= delta * 0.75f;

  if (cutscene_value > 1.0f)
    cutscene_value = 1.0f;
  else if (cutscene_value < 0.0f)
    cutscene_value = 0.0f;
}

void
GameSessionImpl::update_input(float delta)
{
  Uint8 const* keystate = SDL_GetKeyboardState(nullptr);

  // Hacks to play around with the game speed
  if(keystate[SDL_SCANCODE_KP_1])
    game_speed *= 1.0f - delta;

  if(keystate[SDL_SCANCODE_KP_3])
    game_speed *= 1.0f + delta;

  if(keystate[SDL_SCANCODE_KP_5])
    game_speed = 1.0f;
}

void
GameSessionImpl::update_game(float delta)
{
  game_time += delta;
  ScriptManager::current()->update();

  view.update(delta);
  sector->update(delta);

  speech_manager.update(delta);

  switch (fade_state)
  {
    case FADEIN:
      if (fadeout_value <= 0.0f)
      {
        fade_state = RUNNING;
        fadeout_value = 0.0f;
        ScriptManager::current()->fire_wakeup_event(ScriptManager::FADE_DONE);
      }
      else
      {
        fadeout_value -= delta * fade_time;
      }
      break;

    case FADEOUT:
      if (fadeout_value >= 1.0f)
      {
        fadeout_value = 1.0f;
        ScriptManager::current()->fire_wakeup_event(ScriptManager::FADE_DONE);

        switch(next_action)
        {
          case CHANGE_SECTOR_ACTION:
            GameSession::current()->set_sector(filename);
            break;

          case QUIT_ACTION:
            g_app.screen().quit();
            break;

          default:
            break;
        }
      }
      else
      {
        fadeout_value += delta * fade_time;
      }
      break;

    case RUNNING:
      break;
  }
}

void
GameSessionImpl::update_controller(float delta, wstinput::Controller const& controller)
{
  // Handle key presses
  if (controller.button_was_pressed(PDA_BUTTON))
  {
    if (current_gui == &pda)
      current_gui = nullptr;
    else
      current_gui = &pda;
  }
  else if (controller.button_was_pressed(INVENTORY_BUTTON))
  {
    if (current_gui == &inventory)
      current_gui = nullptr;
    else
      current_gui = &inventory;
  }
  else if (controller.button_was_pressed(ESCAPE_BUTTON) ||
           controller.button_was_pressed(PAUSE_BUTTON))
  {
    MenuManager::display_pause_menu();
  }
  else
  {
    if (!current_gui)
    {
      if (Player::current())
        Player::current()->update(controller, delta);
      else if (Doll::current())
        Doll::current()->update(controller, delta);
    }
  }

  if (current_gui)
  {
    current_gui->update(delta, controller);
  }
  else
  {
    // FIXME: route controller to player
  }
}

void
GameSessionImpl::update(float delta, wstinput::Controller const& controller)
{
  update_cutscene(delta);

  delta *= game_speed;

  // Update the game
  if (!pause)
  {
    update_game(delta);
  }

  update_controller(delta, controller);
}

void
GameSession::change_sector(Pathname const& arg_filename)
{
  impl->filename = arg_filename;

  g_app.sound().stop_music();

  if (impl->fade_state == GameSessionImpl::FADEOUT)
  {
    GameSession::current()->set_sector(impl->filename);
  }
  else
  {
    impl->fade_time     = 1.0f;
    impl->fade_state    = GameSessionImpl::FADEOUT;
    impl->next_action   = GameSessionImpl::CHANGE_SECTOR_ACTION;
  }
}

void
GameSession::set_sector(Pathname const& /* FIXME: huh? */)
{
  impl->sector.reset(new Sector(impl->filename));

  impl->sector->activate();
}

void
GameSessionImpl::handle_event(SDL_Event const& event)
{
  switch(event.type)
  {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (event.key.state)
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_1:
            sc.set_render_mask(sc.get_render_mask() ^ wstdisplay::SceneContext::COLORMAP);
            ConsoleLog << "Toggled COLORMAP: " << ((sc.get_render_mask() & wstdisplay::SceneContext::COLORMAP) > 0) << std::endl;
            break;

          case SDLK_2:
            sc.set_render_mask(sc.get_render_mask() ^ wstdisplay::SceneContext::LIGHTMAP);
            ConsoleLog << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & wstdisplay::SceneContext::LIGHTMAP) > 0) << std::endl;
            break;

          case SDLK_3:
            sc.set_render_mask(sc.get_render_mask() ^ wstdisplay::SceneContext::HIGHLIGHTMAP);
            ConsoleLog << "Toggled HIGHLIGHTMAP: " << ((sc.get_render_mask() & wstdisplay::SceneContext::HIGHLIGHTMAP) > 0) << std::endl;
            break;

          case SDLK_4:
            sc.set_render_mask(sc.get_render_mask() ^ wstdisplay::SceneContext::CONTROLMAP);
            ConsoleLog << "Toggled CONTROLMAP: " << ((sc.get_render_mask() & wstdisplay::SceneContext::CONTROLMAP) > 0) << std::endl;
            break;

          case SDLK_5:
            sc.set_render_mask(sc.get_render_mask() ^ wstdisplay::SceneContext::LIGHTMAPSCREEN);
            ConsoleLog << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & wstdisplay::SceneContext::LIGHTMAPSCREEN) > 0) << std::endl;
            break;

          case SDLK_c:
            collision_debug = !collision_debug;
            ConsoleLog << "Collision Debugging " << (collision_debug ? "enabled" : "disabled") << std::endl;
            break;

          case SDLK_F2:
            MenuManager::display_debug_menu();
            break;

          default:
            break;
        }
      }
      break;

    case SDL_MOUSEBUTTONDOWN:
    {
      glm::vec2 real_pos = view.screen_to_world(glm::vec2(event.button.x, event.button.y));
      ConsoleLog << "Click at: " << int(real_pos.x) << ", " << int(real_pos.y) << std::endl;
    }
    break;
  }
}

void
GameSession::quit()
{
  if (impl->fade_state != GameSessionImpl::FADEOUT)
  {
    g_app.sound().stop_music();
    impl->fade_state  = GameSessionImpl::FADEOUT;
    impl->fade_time   = 1.0f;
    impl->next_action = GameSessionImpl::QUIT_ACTION;
  }
}

PDA&
GameSession::get_pda()
{
  return impl->pda;
}

void
GameSession::draw(wstdisplay::GraphicsContext& gc)
{
  impl->draw(gc);
}

void
GameSession::update(float delta, wstinput::Controller const& controller)
{
  impl->update(delta, controller);
}

void
GameSession::handle_event(SDL_Event const& event)
{
  impl->handle_event(event);
}

View*
GameSession::get_view()
{
  return &impl->view;
}

void
GameSession::set_control_state(ControlState state)
{
  switch(state)
  {
    case DIALOG:
      impl->current_gui = &impl->dialog_manager;
      break;

    case CONVERSATION:
      impl->current_gui = &impl->conversation;
      break;

    case GAME:
      impl->current_gui = nullptr;
      break;
  }
}

bool
GameSession::is_active() const
{
  return (impl->current_gui == nullptr);
}

void
GameSession::set_cutscene_mode(bool t)
{
  if (t != impl->cutscene_mode)
  {
    impl->cutscene_mode  = t;
  }
}

void
GameSession::fadeout(float time, surf::Color const& color)
{
  if (time == 0.0f)
  { // directly go to a state of solid color, no fading
    impl->fadeout_value  = 1.0f;
    impl->fade_color  = color;
    impl->fade_state  = GameSessionImpl::FADEOUT;
    impl->next_action = GameSessionImpl::NO_ACTION;
  }
  else
  {
    impl->fade_time   = 1.0f/time;
    impl->fade_color  = color;
    impl->fade_state  = GameSessionImpl::FADEOUT;
    impl->next_action = GameSessionImpl::NO_ACTION;
  }
}

void
GameSession::fadein(float time)
{
  impl->fade_time   = 1.0f/time;
  impl->fade_state  = GameSessionImpl::FADEIN;
  impl->next_action = GameSessionImpl::NO_ACTION;
}

wstdisplay::SceneContext*
GameSession::get_scene_context()
{
  return &(impl->sc);
}

/* EOF */
