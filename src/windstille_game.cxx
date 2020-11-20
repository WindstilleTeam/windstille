//  $Id: windstille_game.cxx,v 1.33 2003/11/13 12:59:42 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <math.h>
#include <ClanLib/gl.h>
#include <sstream>

#include "fonts.hxx"
#include "game_world.hxx"
#include "gameobj.hxx"
#include "player.hxx"
#include "animation_obj.hxx"
#include "tile_map.hxx"
#include "dog.hxx"
#include "music_manager.hxx"
#include "power_up.hxx"
#include "bonus_flyer.hxx"
#include "display.hxx"
#include "player_view.hxx"
#include "energiebar.hxx"
#include "background.hxx"
#include "gui_manager.hxx"
#include "view_component.hxx"
#include "dialog_manager.hxx"
#include "windstille_main.hxx"
#include "screenshot.hxx"
#include "input/input_manager.hxx"

#include "guile_gameobj_factory.hxx"
#include "windstille_game.hxx"

using namespace Windstille;

WindstilleGame* WindstilleGame::current_ = 0; 

WindstilleGame::WindstilleGame(const std::string& arg_filename)
  : frames(0), filename (arg_filename)
{
  current_ = this;
  world = new GameWorld(filename);

  state = FADEIN;
  fadeout_value = 0;

  control_state = GAME;

  manager = new GUIManager();
}

WindstilleGame::WindstilleGame(GameWorld* w)
{
  current_ = this;
  world = w;
  control_state = GAME;

  new GUIManager();
}

WindstilleGame::~WindstilleGame()
{
  delete GUIManager::current();
  delete world;
}

void
WindstilleGame::draw_game()
{
  background->draw();

  // Draw the world
  view->draw ();

  // Draw HUD
  energiebar->draw();

  switch (control_state)
    {
    case DIALOG:
      dialog_manager->draw();      
      break;
    default:
      break;
    }
  
  // Draw Logo
  if (0)
    {     
      //logo.set_blend_func(blend_src_alpha, blend_one);
      logo.set_alpha(cos(blink)*0.5f + 0.5f);
      logo.draw(800 - 302, 600 - 95);
      logo_black.draw(800 - 302, 600 - 95);
    }
}

void
WindstilleGame::draw()
{
  draw_game();

  // Draw debug GUI
  GUIManager::current()->draw();

  switch (state)
    {
    case FADEOUT:
      CL_Display::fill_rect(CL_Rect(0, 0, 
                                    CL_Display::get_width(), CL_Display::get_height()),
                            CL_Color(0,0,0, std::min(int(fadeout_value*255), 255)));
      break;
    case FADEIN:
      CL_Display::fill_rect(CL_Rect(0, 0, 
                                    CL_Display::get_width(), CL_Display::get_height()),
                            CL_Color(0,0,0, 255-std::min(int(fadeout_value*255), 255)));
      break;

    default:
      break;
    }

  if (player->get_movement_state() == Player::DEAD)
    {
      CL_Font font = Fonts::dialog;
      font.set_alignment(origin_bottom_center);
      font.draw(CL_Display::get_width()/2, 200,
                "..:: Press Fire to restart ::..");
    }

  CL_Display::flip();

  if (!main_app.screenshot_dir.empty())
    {
      std::stringstream filename;
      filename << main_app.screenshot_dir;
      filename.width(8);
      filename.fill('0');
      filename << frames;
      filename << ".ppm";
      Screenshot::write_screenshot_pnm(filename.str());
    }
  ++frames;
}

void
WindstilleGame::update(float delta)
{
  InputManager::update(delta);
  delta *= game_speed;

  view->update (delta);

  switch (state)
    {
    case FADEIN:
      if (fadeout_value > 1.0f)
        state = RUNNING;
      fadeout_value += delta;
      break;
    case FADEOUT:
      if (fadeout_value > 1.0f)
        Screen::quit();

      fadeout_value += delta;
      break;
    case RUNNING:
      switch (control_state) 
        {
        case DIALOG:
          dialog_manager->update(delta);
          break;
        case GAME:
          world->update (delta);
          energiebar->update(delta);
          break;
        }
      break;
    }
  
  // Debug stuff
  if (CL_Keyboard::get_keycode(CL_KEY_F1))
    GUIManager::current()->show();
  else if (CL_Keyboard::get_keycode(CL_KEY_F2))
    GUIManager::current()->hide();
  
  if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    quit();
  
  InputManager::clear();

  blink += delta * 3.141f;
  GUIManager::current()->update();
}

void
WindstilleGame::on_startup ()
{ 
  CL_Display::get_current_window()->hide_cursor();

  MusicManager::current()->play(datadir + "/music/techdemo.ogg", true);
  blink = 0.0f;

  GameObj::set_world (world);
  
  player = new Player();
  view   = new PlayerView(player);
  
  // FIXME: Mem leak
  new ViewComponent(GUIManager::current()->get_component(), view);

  energiebar = new Energiebar();
  background = new Background();
  dialog_manager = new DialogManager();

  world->add_player(player);

  scm_c_primitive_load((datadir + "/game.scm").c_str());

  logo       = CL_Sprite("logo", resources);
  portrait   = CL_Sprite("hero/portrait", resources);
  logo_black = CL_Sprite("logo_black", resources);

  world->on_startup();
}

void
WindstilleGame::on_shutdown ()
{
  MusicManager::current()->stop();

  delete energiebar;
  delete background;
  delete view;
  delete dialog_manager;
}

void
WindstilleGame::quit()
{
  fadeout_value = 0;
  state = FADEOUT;
  MusicManager::current()->stop();
}

/* EOF */
