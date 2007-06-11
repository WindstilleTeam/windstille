/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include "config.hpp"
#include "console.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "gui/gui_manager.hpp"
#include "gui/root_component.hpp"
#include "screen_manager.hpp"
#include "gui/menu_component.hpp"
#include "game_session.hpp"
#include "title_screen.hpp"

TitleScreen::TitleScreen()
{
  background = Sprite("images/titlescreen.sprite");
  on_start();
}

TitleScreen::~TitleScreen()
{
}

void
TitleScreen::on_start()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(Vector(400, 250), Sizef(200, 500)), false,
                                          manager->get_root());

  menu->set_font(Fonts::vera20);

  ButtonMenuItem* select_scenario_button = new ButtonMenuItem(menu,  "Select Scenario");
  slots.push_back(select_scenario_button->sig_click().connect(this, &TitleScreen::menu_select_scenario));
  menu->add_item(select_scenario_button);

  ButtonMenuItem* options_button = new ButtonMenuItem(menu,  "Options");
  slots.push_back(options_button->sig_click().connect(this, &TitleScreen::menu_options));
  menu->add_item(options_button);

  ButtonMenuItem* credits_button = new ButtonMenuItem(menu,  "Credits");
  slots.push_back(credits_button->sig_click().connect(this, &TitleScreen::menu_credits));
  menu->add_item(credits_button);

  ButtonMenuItem* quit_button = new ButtonMenuItem(menu,  "Quit");
  slots.push_back(quit_button->sig_click().connect(this, &TitleScreen::menu_quit));
  menu->add_item(quit_button);
  // End: Option Menu

  manager->get_root()->set_child(menu);

  screen_manager.push_overlay(manager);
}

void
TitleScreen::draw()
{
  background.draw(Vector(0, 0));
}

void
TitleScreen::update(float delta, const Controller& controller)
{
  background.update(delta);
}

void
TitleScreen::handle_event(const SDL_Event& )
{
  
}

void
TitleScreen::menu_start_game()
{
  screen_manager.set_screen(new GameSession("levels/newformat2.wst"));
  screen_manager.pop_overlay();
}

void
TitleScreen::menu_select_scenario()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  MenuComponent* menu = new MenuComponent(Rectf(Vector(500-200, 250), Sizef(400, 500)), true,
                                          manager->get_root());

  menu->set_font(Fonts::vera20);

  std::vector<std::string> scenarios;
  scenarios.push_back("levels/apartment.wst");
  scenarios.push_back("levels/bluestone.wst");
  scenarios.push_back("levels/forest.wst");
  scenarios.push_back("levels/industrial.wst");
  scenarios.push_back("levels/intro.wst");
  scenarios.push_back("levels/virtualreality.wst");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      ButtonMenuItem* scenario_button = new ButtonMenuItem(menu,  *i);

      slots.push_back(scenario_button->sig_click().connect<TitleScreen, std::string>
                      (this, &TitleScreen::menu_start_scenario, 
                       std::string(*i)));

      menu->add_item(scenario_button);
    }

  manager->get_root()->set_child(menu);

  screen_manager.push_overlay(manager);
}

void
TitleScreen::menu_options()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(Vector(500-200, 250), Sizef(400, 500)), true,
                                          manager->get_root());

  menu->set_font(Fonts::vera20);

  SliderMenuItem* music_volume_item = new SliderMenuItem(menu,  "Music Volume", 100, 0, 100, 10);
  menu->add_item(music_volume_item);

  SliderMenuItem* sfx_volume_item   = new SliderMenuItem(menu,  "SFX Volume",   100, 0, 100, 10);
  menu->add_item(sfx_volume_item);

  SliderMenuItem* voice_volume_item = new SliderMenuItem(menu,  "Voice Volume", 100, 0, 100, 10);
  menu->add_item(voice_volume_item);

  EnumMenuItem* aspect_item = new EnumMenuItem(menu,  "Aspect Ratio", 0);
  aspect_item->add_pair(0, "4:3");
  aspect_item->add_pair(1, "5:4");
  aspect_item->add_pair(2, "16:9");
  aspect_item->add_pair(3, "16:10");
  aspect_item->add_pair(4, "letterbox");
  menu->add_item(aspect_item);
  
  EnumMenuItem* show_fps_item = new EnumMenuItem(menu,  "Show FPS", config.get_bool("show-fps"));
  show_fps_item->add_pair(1, "on");
  show_fps_item->add_pair(0, "off");
  menu->add_item(show_fps_item);
  slots.push_back(show_fps_item->sig_change().connect(this, &TitleScreen::menu_show_fps));

  EnumMenuItem* fullscreen_item = new EnumMenuItem(menu,  "Fullscreen", config.get_bool("fullscreen"));
  fullscreen_item->add_pair(1, "on");
  fullscreen_item->add_pair(0, "off");
  menu->add_item(fullscreen_item);
  slots.push_back(fullscreen_item->sig_change().connect(this, &TitleScreen::menu_fullscreen));

  manager->get_root()->set_child(menu);

  screen_manager.push_overlay(manager);
}

void
TitleScreen::menu_credits()
{
  console << "Credits clicked" << std::endl;
}

void
TitleScreen::menu_quit()
{
  //GameSession::current()->quit();
  screen_manager.pop_overlay();
  screen_manager.quit();
}

void
TitleScreen::menu_start_scenario(std::string scenario)
{
  std::cout << "Starting: " << scenario << std::endl;
  screen_manager.set_screen(new GameSession(scenario));
  screen_manager.clear_overlay();
 }

void
TitleScreen::menu_show_fps(int i)
{
  config.set_bool("show-fps", i);
}

void
TitleScreen::menu_fullscreen(int i)
{
  config.set_bool("fullscreen", i);
  Display::set_fullscreen(config.get_bool("fullscreen"));
}

/* EOF */
