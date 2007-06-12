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

#include "config.hpp"
#include "console.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "game_session.hpp"
#include "gui/group_component.hpp"
#include "gui/gui_manager.hpp"
#include "gui/menu_component.hpp"
#include "gui/root_component.hpp"
#include "gui/text_view.hpp"
#include "particle_viewer.hpp"
#include "screen_manager.hpp"
#include "sector.hpp"
#include "sprite3d/manager.hpp"
#include "sprite3dview.hpp"
#include "menu_manager.hpp"

MenuManager menu_manager;

MenuManager::MenuManager()
{
}

void
MenuManager::display_option_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-250, 300-170), Sizef(500, 340)), 
                                             "Options",
                                             manager->get_root());

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(), true,
                                          group);
  group->pack(menu);

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
  show_fps_item->add_pair(0, "off");
  show_fps_item->add_pair(1, "on");
  menu->add_item(show_fps_item);
  slots.push_back(show_fps_item->sig_change().connect(this, &MenuManager::menu_show_fps));

  EnumMenuItem* fullscreen_item = new EnumMenuItem(menu,  "Fullscreen", config.get_bool("fullscreen"));
  fullscreen_item->add_pair(0, "off");
  fullscreen_item->add_pair(1, "on");
  menu->add_item(fullscreen_item);
  slots.push_back(fullscreen_item->sig_change().connect(this, &MenuManager::menu_fullscreen));

  EnumMenuItem* difficulty_item = new EnumMenuItem(menu,  "Difficulty", 1);
  difficulty_item->add_pair(0, "easy");
  difficulty_item->add_pair(1, "medium");
  difficulty_item->add_pair(2, "hard");
  menu->add_item(difficulty_item);

  manager->get_root()->add_child(group);
  group->layout();
  screen_manager.push_overlay(manager);
}

void
MenuManager::display_main_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* text_group = new GroupComponent(Rectf(10, 500+20, 800-10, 600-10),
                                                  "",
                                                  manager->get_root());

  TextView* text = new TextView(Rectf(), text_group);
  text_group->pack(text);
  text->set_font(Fonts::vera12);
  text->set_text("Copyright (C) 2007 Ingo Ruhnke &lt;grumbel@gmx.de&gt;\n"
                 "\n"
                 "This program is free software; you can redistribute it and/or "
                 "modify it under the terms of the GNU General Public License "
                 "as published by the Free Software Foundation; either version 2 "
                 "of the License, or (at your option) any later version.");
  manager->get_root()->add_child(text_group);

  GroupComponent* group = new GroupComponent(Rectf(Vector(400, 200), Sizef(200, 294)),
                                             "",
                                             manager->get_root());

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(), false,
                                          group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  ButtonMenuItem* select_scenario_button = new ButtonMenuItem(menu,  "Select Scenario");
  slots.push_back(select_scenario_button->sig_click().connect(this, &MenuManager::display_scenario_menu));
  menu->add_item(select_scenario_button);

  ButtonMenuItem* model_viewer_button = new ButtonMenuItem(menu,  "Model Viewer");
  slots.push_back(model_viewer_button->sig_click().connect(this, &MenuManager::display_models_menu));
  menu->add_item(model_viewer_button);

  ButtonMenuItem* particles_button = new ButtonMenuItem(menu,  "Particle Systems");
  slots.push_back(particles_button->sig_click().connect(this, &MenuManager::display_particle_menu));
  menu->add_item(particles_button);

  ButtonMenuItem* options_button = new ButtonMenuItem(menu,  "Options");
  slots.push_back(options_button->sig_click().connect(this, &MenuManager::display_option_menu));
  menu->add_item(options_button);

  ButtonMenuItem* credits_button = new ButtonMenuItem(menu,  "Credits");
  slots.push_back(credits_button->sig_click().connect(this, &MenuManager::display_credits));
  menu->add_item(credits_button);

  ButtonMenuItem* help_button = new ButtonMenuItem(menu,  "Help");
  slots.push_back(help_button->sig_click().connect(this, &MenuManager::display_help));
  menu->add_item(help_button);

  ButtonMenuItem* quit_button = new ButtonMenuItem(menu,  "Quit");
  slots.push_back(quit_button->sig_click().connect(this, &MenuManager::menu_quit));
  menu->add_item(quit_button);
  // End: Option Menu

  manager->get_root()->add_child(group);
  group->layout();
  screen_manager.push_overlay(manager);
}

void
MenuManager::display_pause_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-200, 300-170), Sizef(400, 340)), 
                                             "Pause Menu",
                                             manager->get_root());

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(Vector(400-150, 200), Sizef(300, 500)), true,
                                          group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  ButtonMenuItem* continue_button = new ButtonMenuItem(menu,  "Resume Game");
  slots.push_back(continue_button->sig_click().connect(this, &MenuManager::menu_continue));
  menu->add_item(continue_button);

  ButtonMenuItem* select_scenario_button = new ButtonMenuItem(menu,  "Select Scenario");
  slots.push_back(select_scenario_button->sig_click().connect(this, &MenuManager::display_scenario_menu));
  menu->add_item(select_scenario_button);

  ButtonMenuItem* options_button = new ButtonMenuItem(menu,  "Options");
  slots.push_back(options_button->sig_click().connect(this, &MenuManager::display_option_menu));
  menu->add_item(options_button);

  ButtonMenuItem* debug_button = new ButtonMenuItem(menu,  "Debug");
  slots.push_back(debug_button->sig_click().connect(this, &MenuManager::display_debug_menu));
  menu->add_item(debug_button);

  ButtonMenuItem* credits_button = new ButtonMenuItem(menu,  "Credits");
  slots.push_back(credits_button->sig_click().connect(this, &MenuManager::display_credits));
  menu->add_item(credits_button);

  ButtonMenuItem* help_button = new ButtonMenuItem(menu,  "Help");
  slots.push_back(help_button->sig_click().connect(this, &MenuManager::display_help));
  menu->add_item(help_button);

  ButtonMenuItem* quit_button = new ButtonMenuItem(menu,  "Return to Title Screen");
  slots.push_back(quit_button->sig_click().connect(this, &MenuManager::menu_exit));
  menu->add_item(quit_button);
  // End: Option Menu

  manager->get_root()->add_child(group);

  screen_manager.push_overlay(manager); 
}

void
MenuManager::display_models_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-250, 0), Sizef(500, 600)), 
                                             "Select Model",
                                             manager->get_root());

  MenuComponent* menu = new MenuComponent(Rectf(), true, group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::vector<std::string> models;
  models.push_back("models/characters/bob/bob.wsprite");
  models.push_back("models/characters/jane/jane.wsprite");
  models.push_back("models/characters/yagor/yagor.wsprite");
  models.push_back("models/objects/pistol/pistol.wsprite");
  models.push_back("models/vehicles/train/train.wsprite"); 
  models.push_back("models/characters/powersuit/powersuit.wsprite");
  models.push_back("models/characters/spider/spider.wsprite");
  models.push_back("models/objects/grenade/grenade.wsprite");
  models.push_back("models/vehicles/shuttle/shuttle.wsprite");
  models.push_back("models/characters/vrdummy/vrdummy.wsprite");
  models.push_back("models/characters/monster/monster.wsprite");
  models.push_back("models/characters/sophie/sophie.wsprite");

  for(std::vector<std::string>::iterator i = models.begin(); i != models.end(); ++i)
    {
      ButtonMenuItem* scenario_button = new ButtonMenuItem(menu,  *i);

      slots.push_back(scenario_button->sig_click().connect<MenuManager, std::string>
                      (this, &MenuManager::menu_show_model, 
                       std::string(*i)));

      menu->add_item(scenario_button);
    }

  manager->get_root()->add_child(group);

  screen_manager.push_overlay(manager);  
}

void
MenuManager::display_particle_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-200, 300-170), Sizef(400, 340)), 
                                             "Particle Systems",
                                             manager->get_root());

  MenuComponent* menu = new MenuComponent(Rectf(), true, group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::vector<std::string> scenarios;
  scenarios.push_back("particlesystems/fire.particles");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      ButtonMenuItem* scenario_button = new ButtonMenuItem(menu,  *i);

      slots.push_back(scenario_button->sig_click().connect<MenuManager, std::string>
                      (this, &MenuManager::menu_show_particle_system, 
                       std::string(*i)));

      menu->add_item(scenario_button);
    }

  manager->get_root()->add_child(group);

  screen_manager.push_overlay(manager); 
}

void
MenuManager::display_scenario_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-200, 300-170), Sizef(400, 340)), 
                                             "Select Scenario",
                                             manager->get_root());

  MenuComponent* menu = new MenuComponent(Rectf(), true, group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::vector<std::string> scenarios;
  scenarios.push_back("levels/apartment.wst");
  scenarios.push_back("levels/bluestone.wst");
  scenarios.push_back("levels/forest.wst");
  scenarios.push_back("levels/industrial.wst");
  scenarios.push_back("levels/intro.wst");
  scenarios.push_back("levels/newformat2.wst");
  scenarios.push_back("levels/virtualreality.wst");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      ButtonMenuItem* scenario_button = new ButtonMenuItem(menu,  *i);

      slots.push_back(scenario_button->sig_click().connect<MenuManager, std::string>
                      (this, &MenuManager::menu_start_scenario, 
                       std::string(*i)));

      menu->add_item(scenario_button);
    }

  manager->get_root()->add_child(group);

  screen_manager.push_overlay(manager); 
}

void
MenuManager::display_debug_menu()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-250, 300-170), Sizef(500, 340)), 
                                             "Debug",
                                             manager->get_root());

  // Begin Main Menu
  MenuComponent* menu = new MenuComponent(Rectf(), true, group);
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  Color amb = Sector::current()->get_ambient_light();

  SliderMenuItem* r_ambient_light_item = new SliderMenuItem(menu,  "Ambient Light (Red)", int(amb.r*100), 0, 100, 10);
  slots.push_back(r_ambient_light_item->sig_change().connect(this, &MenuManager::menu_ambient_light, 0));
  menu->add_item(r_ambient_light_item);

  SliderMenuItem* g_ambient_light_item = new SliderMenuItem(menu,  "Ambient Light (Green)", int(amb.g*100), 0, 100, 10);
  slots.push_back(g_ambient_light_item->sig_change().connect(this, &MenuManager::menu_ambient_light, 1));
  menu->add_item(g_ambient_light_item);

  SliderMenuItem* b_ambient_light_item = new SliderMenuItem(menu,  "Ambient Light (Blue)", int(amb.b*100), 0, 100, 10);
  slots.push_back(b_ambient_light_item->sig_change().connect(this, &MenuManager::menu_ambient_light, 2));
  menu->add_item(b_ambient_light_item);

  manager->get_root()->add_child(group);

  screen_manager.push_overlay(manager); 
}

void
MenuManager::display_help()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-250, 300-200), Sizef(500, 400)), 
                                             "Help",
                                             manager->get_root());

  TextView* text = new TextView(Rectf(), group);
  text->set_font(Fonts::vera12);
  text->set_text("This is a tech-demo of Windstille. Its not meant "
                 "to be playable in any way except a bit of walking around. "
                 "It provides nothing to accomplish, just a few scenarios to "
                 "load and look at. Enjoy what works, don't complain if something "
                 "doesn't, since most stuff simply won't work anyway.\n"
                 "\n"
                 "Key Bindings\n"
                 "============\n"
                 "F1 - open the console\n"
                 "F6 - mouse release\n"
                 "F7 - mouse grap\n"
                 "F8 - run gui test (broken)\n"
                 "F9 - configure input\n"
                 "F11 - toggle fullscreen\n"
                 "F12 - do a screenshot\n"
                 "\n"
                 "cursor keys - walk\n"
                 "d - run/cancel\n"
                 "s - jump/ok\n"
                 );
  text->set_active(true);

  group->pack(text);

  manager->get_root()->add_child(group);
  screen_manager.push_overlay(manager);
}

void
MenuManager::display_credits()
{
  using namespace GUI;
  GUIManager* manager = new GUIManager();

  GroupComponent* group = new GroupComponent(Rectf(Vector(400-250, 300-200), Sizef(500, 400)), 
                                             "Credits",
                                             manager->get_root());

  TextView* text = new TextView(Rectf(), group);
  text->set_font(Fonts::vera12);
  text->set_text("Programming\n"
                 "===========\n"
                 "\n"
                 "  Ingo Ruhnke - Grumbel - &lt;grumbel@gmx.de&gt;\n"
                 "  Matthias Braun - MatzeB - matze@braunis.de\n"
                 "  James Gregory -\n"
                 "  David Kamphausen - Godrin - &lt;david.kamphausen@web.de&gt;\n"
                 "  Mark Dillavou - Line72- &lt;line72@line72.net&gt;\n"
                 "\n"
                 "Graphics\n"
                 "========\n"
                 "  Ingo Ruhnke - Grumbel - &lt;grumbel@gmx.de&gt;\n"
                 "  Ken Hirsch - quickflash - &lt;khirsch11414@yahoo.com&gt;\n"
                 "\n"
                 "0.2.0 Win32 Build\n"
                 "==================\n"
                 "  Robert Konrad &lt;rkon@gmx.de&gt;\n"
                 "\n"
                 "Music\n"
                 "=====\n"
                 "  Ralph Weinert &lt;r.weinert@bluewin.ch&gt;\n"
                 "  Marek Moeckel - Wansti - &lt;wansti@gmx.de&gt;\n");
  text->set_active(true);

  group->pack(text);

  manager->get_root()->add_child(group);
  screen_manager.push_overlay(manager);
}
  
// Callbacks

void
MenuManager::menu_show_model(std::string model)
{
  Sprite3DView* sprite3dview = new Sprite3DView();

  sprite3dview->set_model(model);

  // Launching Sprite3DView instead of game
  screen_manager.push_screen(sprite3dview);
  screen_manager.clear_overlay();
}

void
MenuManager::menu_show_particle_system(std::string file)
{
  ParticleViewer* particle_viewer = new ParticleViewer();
  particle_viewer->load(file);

  screen_manager.push_screen(particle_viewer);
  screen_manager.clear_overlay();
}

void
MenuManager::menu_start_game()
{
  screen_manager.push_screen(new GameSession("levels/newformat2.wst"));
  screen_manager.pop_overlay();
}

void
MenuManager::menu_quit()
{
  //GameSession::current()->quit();
  screen_manager.pop_overlay();
  screen_manager.quit();
}

void
MenuManager::menu_start_scenario(std::string scenario)
{
  std::cout << "Starting: " << scenario << std::endl;
  screen_manager.push_screen(new GameSession(scenario));
  screen_manager.clear_overlay();
 }

void
MenuManager::menu_show_fps(int i)
{
  config.set_bool("show-fps", i);
}

void
MenuManager::menu_fullscreen(int i)
{
  config.set_bool("fullscreen", i);
  Display::set_fullscreen(config.get_bool("fullscreen"));
}

void
MenuManager::menu_continue()
{
  screen_manager.pop_overlay();
}

void
MenuManager::menu_exit()
{
  // FIXME: Make this return to the title screen
  screen_manager.pop_overlay();
  screen_manager.pop_screen(); 
}

void
MenuManager::menu_ambient_light(int i, int component)
{
  if (Sector::current())
    {
      Color amb = Sector::current()->get_ambient_light();

      if (component == 0)
        amb.r = i / 100.0f;
      else if (component == 1)
        amb.g = i / 100.0f;
      else if (component == 2)
        amb.b = i / 100.0f;

      Sector::current()->set_ambient_light(amb);
    }
}

/* EOF */
