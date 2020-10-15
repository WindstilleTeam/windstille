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

#ifndef HEADER_WINDSTILLE_SCREEN_GAME_SESSION_HPP
#define HEADER_WINDSTILLE_SCREEN_GAME_SESSION_HPP

#include <string>
#include <squirrel.h>

#include "app/console.hpp"
#include "app/globals.hpp"
#include "display/scene_context.hpp"
#include "hud/controller_help_window.hpp"
#include "screen/screen.hpp"
#include "sprite2d/sprite.hpp"
#include "util/currenton.hpp"

class PDA;
class EnergyBar;
class View;
class Sector;
class DialogManager;
class Conversation;
class Inventory;
class Pathname;

class GameSessionImpl;

class GameSession : public Screen,
                    public Currenton<GameSession>
{
public:
  enum ControlState { DIALOG, GAME, CONVERSATION };

  GameSession(const Pathname& arg_filename);
  ~GameSession() override;

  View* get_view();

  /** Return true when the gameplay is currently active, return false
      if PDA, Conversation or other things are active */
  bool is_active() const;

  void set_control_state(ControlState state);

  /** Switches the sector instantly without fadeout */
  void set_sector(const Pathname& filename);

  /** Fades out then switches sectors and fades in again */
  void change_sector(const Pathname& filename);

  /** Set the GameSession into a mode for cutscenes, this means for
      most part to switch into a 16:9 viewmode with black borders */
  void set_cutscene_mode(bool t);

  /** \a time Time in seconds till the fade is complete
      \a color Color to which the screen should fade */
  void fadeout(float time, const Color& color);
  void fadein(float time);

  void draw(GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;
  void handle_event(const SDL_Event& event) override;

  PDA& get_pda();

  void quit();

  SceneContext* get_scene_context();

private:
  std::shared_ptr<GameSessionImpl> impl;
};

#endif

/* EOF */
