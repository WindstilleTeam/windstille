/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef GAME_SESSION_HXX
#define GAME_SESSION_HXX

#include <string>
#include <squirrel.h>
#include "display/scene_context.hpp"
#include "console.hpp"
#include "screen.hpp"
#include "controller_help_window.hpp"
#include "sprite2d/sprite.hpp"
#include "globals.hpp"

class PDA;
class EnergyBar;
class View;
class Sector;
class Player;
class DialogManager;
class Conversation;
class Inventory;

class GameSessionImpl;

class GameSession : public Screen
{
public:
  enum ControlState { DIALOG, GAME, CONVERSATION };
private:
  static GameSession* current_; 
public:
  static GameSession* current() { return current_; } 

  GameSession (const std::string& arg_filename);
  ~GameSession ();

  View* get_view();

  /** Return true when the gameplay is currently active, return false
      if PDA, Conversation or other things are active */
  bool is_active() const;

  void set_control_state(ControlState state);

  /** Switches the sector instantly without fadeout */
  void set_sector(const std::string& filename);

  /** Fades out then switches sectors and fades in again */
  void change_sector(const std::string& filename);

  /** Set the GameSession into a mode for cutscenes, this means for
      most part to switch into a 16:9 viewmode with black borders */
  void set_cutscene_mode(bool t);

  /** \a time Time in seconds till the fade is complete 
      \a color Color to which the screen should fade */
  void fadeout(float time, const Color& color);
  void fadein(float time);

  void draw();
  void update(float delta, const Controller& controller);
  void handle_event(const SDL_Event& event);

  PDA& get_pda();

  void quit();

  SceneContext* get_scene_context();
private:
  SharedPtr<GameSessionImpl> impl;
};

#endif

/* EOF */
