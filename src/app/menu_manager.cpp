/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <memory>
#include <boost/bind.hpp>
#include "config.hpp"
#include "sound/sound_manager.hpp"
#include "console.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "screen/game_session.hpp"
#include "gui/group_component.hpp"
#include "gui/gui_manager.hpp"
#include "gui/menu_component.hpp"
#include "gui/root_component.hpp"
#include "gui/text_view.hpp"
#include "screen/particle_viewer.hpp"
#include "screen/screen_manager.hpp"
#include "engine/sector.hpp"
#include "sprite3d/manager.hpp"
#include "screen/sprite3dview.hpp"
#include "screen/geometry_test.hpp"
#include "screen/armature_test.hpp"
#include "screen/navigation_test.hpp"
#include "gui/menu_item.hpp"
#ifdef HAVE_CWIID
#include "input/wiimote.hpp"
#endif
#include "app/windstille.hpp"
#include "menu_manager.hpp"

MenuManager menu_manager;

MenuManager::MenuManager()
{
}

void
MenuManager::display_option_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-250, 300-170), Sizef(500, 340)), 
                                                         "Options",
                                                         manager->get_root()));

  // Begin Menu
  MenuComponent* menu = new MenuComponent(Rectf(), true, group.get());
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::auto_ptr<SliderMenuItem> music_volume_item(new SliderMenuItem(menu,  "Master Volume",   
                                                                     config.get_int("master-volume"), 0, 100, 10));
  music_volume_item->sig_change().connect(boost::bind(&MenuManager::menu_music_volume, this, _1));
  menu->add_item(music_volume_item.release());

  std::auto_ptr<SliderMenuItem> sfx_volume_item(new SliderMenuItem(menu,  "SFX Volume",   100, 0, 100, 10));
  menu->add_item(sfx_volume_item.release());

  std::auto_ptr<SliderMenuItem> voice_volume_item(new SliderMenuItem(menu,  "Voice Volume", 100, 0, 100, 10));
  menu->add_item(voice_volume_item.release());

  std::auto_ptr<EnumMenuItem> aspect_item(new EnumMenuItem(menu,  "Aspect Ratio", 0));
  aspect_item->add_pair(0, "4:3");
  aspect_item->add_pair(1, "5:4");
  aspect_item->add_pair(2, "16:9");
  aspect_item->add_pair(3, "16:10");
  aspect_item->add_pair(4, "letterbox");
  menu->add_item(aspect_item.release());
  
  std::auto_ptr<EnumMenuItem> show_fps_item(new EnumMenuItem(menu,  "Show FPS", config.get_bool("show-fps")));
  show_fps_item->add_pair(0, "off");
  show_fps_item->add_pair(1, "on");
  show_fps_item->sig_change().connect(boost::bind(&MenuManager::menu_show_fps, this, _1));
  menu->add_item(show_fps_item.release());

  std::auto_ptr<EnumMenuItem> fullscreen_item(new EnumMenuItem(menu,  "Fullscreen", config.get_bool("fullscreen")));
  fullscreen_item->add_pair(0, "off");
  fullscreen_item->add_pair(1, "on");
  fullscreen_item->sig_change().connect(boost::bind(&MenuManager::menu_fullscreen, this, _1));
  menu->add_item(fullscreen_item.release());

  std::auto_ptr<EnumMenuItem> difficulty_item(new EnumMenuItem(menu,  "Difficulty", 1));
  difficulty_item->add_pair(0, "easy");
  difficulty_item->add_pair(1, "medium");
  difficulty_item->add_pair(2, "hard");
  menu->add_item(difficulty_item.release());

  std::auto_ptr<SliderMenuItem> gamma_item(new SliderMenuItem(menu,  "Gamma",  100, 10, 200, 10));
  gamma_item->sig_change().connect(boost::bind(&MenuManager::menu_gamma, this, _1));
  menu->add_item(gamma_item.release());

#ifdef HAVE_CWIID
  if (wiimote)
    {
      std::auto_ptr<ButtonMenuItem> wiimote_button(new ButtonMenuItem(menu,  "Try to Connect Wiimote"));
      wiimote_button->sig_click().connect(boost::bind(&MenuManager::menu_wiimote, this));
      menu->add_item(wiimote_button.release());
    }
#endif

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_main_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> text_group(new GroupComponent(Rectf(10, 500, 800-10, 600-10),
                                                              "",
                                                              manager->get_root()));

  TextView* text = new TextView(Rectf(), text_group.get());
  text_group->pack(text);
  text->set_font(Fonts::vera12);
  text->set_text("Windstille " WINDSTILLE_VERSION " - Copyright (C) 2009 Ingo Ruhnke &lt;grumbel@gmx.de&gt;\n"
                 "\n"
                 "This program is free software: you can redistribute it and/or modify "
                 "it under the terms of the GNU General Public License as published by "
                 "the Free Software Foundation, either version 3 of the License, or "
                 "(at your option) any later version.");

  manager->get_root()->add_child(text_group.release());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-20, 200), Sizef(250, 254)),
                                                         "",
                                                         manager->get_root()));

  // Begin Menu
  MenuComponent* menu = new MenuComponent(Rectf(), false, group.get());
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::auto_ptr<ButtonMenuItem> select_scenario_button(new ButtonMenuItem(menu,  "Select Scenario"));
  select_scenario_button->sig_click().connect(boost::bind(&MenuManager::display_scenario_menu, this));
  menu->add_item(select_scenario_button.release());

  std::auto_ptr<ButtonMenuItem> navigation_test_button(new ButtonMenuItem(menu,  "Navigation Test"));
  navigation_test_button->sig_click().connect(boost::bind(&MenuManager::menu_show_navigation_test, this));
  menu->add_item(navigation_test_button.release());

  std::auto_ptr<ButtonMenuItem> armature_test_button(new ButtonMenuItem(menu,  "Armature Test"));
  armature_test_button->sig_click().connect(boost::bind(&MenuManager::menu_show_armature_test, this));
  menu->add_item(armature_test_button.release());

  std::auto_ptr<ButtonMenuItem> geometry_test_button(new ButtonMenuItem(menu,  "Geometry Test"));
  geometry_test_button->sig_click().connect(boost::bind(&MenuManager::menu_show_geometry_test, this));
  menu->add_item(geometry_test_button.release());

  std::auto_ptr<ButtonMenuItem> model_viewer_button(new ButtonMenuItem(menu,  "Model Viewer"));
  model_viewer_button->sig_click().connect(boost::bind(&MenuManager::display_models_menu, this));
  menu->add_item(model_viewer_button.release());

  std::auto_ptr<ButtonMenuItem> particles_button(new ButtonMenuItem(menu,  "Particle Systems"));
  particles_button->sig_click().connect(boost::bind(&MenuManager::display_particle_menu, this));
  menu->add_item(particles_button.release());

  std::auto_ptr<ButtonMenuItem> options_button(new ButtonMenuItem(menu,  "Options"));
  options_button->sig_click().connect(boost::bind(&MenuManager::display_option_menu, this));
  menu->add_item(options_button.release());

  std::auto_ptr<ButtonMenuItem> credits_button(new ButtonMenuItem(menu,  "Credits"));
  credits_button->sig_click().connect(boost::bind(&MenuManager::display_credits, this));
  menu->add_item(credits_button.release());

  std::auto_ptr<ButtonMenuItem> help_button(new ButtonMenuItem(menu,  "Help"));
  help_button->sig_click().connect(boost::bind(&MenuManager::display_help, this));
  menu->add_item(help_button.release());

  std::auto_ptr<ButtonMenuItem> quit_button(new ButtonMenuItem(menu,  "Quit"));
  quit_button->sig_click().connect(boost::bind(&MenuManager::menu_quit, this));
  menu->add_item(quit_button.release());
  // End: Option Menu

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_pause_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-200, 300-170), Sizef(400, 300)), 
                                                         "Pause Menu",
                                                         manager->get_root()));

  // Begin Menu
  MenuComponent* menu = new MenuComponent(Rectf(Vector2f(400-150, 200), Sizef(300, 500)), true,
                                          group.get());
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::auto_ptr<ButtonMenuItem> continue_button(new ButtonMenuItem(menu,  "Resume Game"));
  continue_button->sig_click().connect(boost::bind(&MenuManager::menu_continue, this));
  menu->add_item(continue_button.release());

  //  std::auto_ptr<ButtonMenuItem> select_scenario_button(new ButtonMenuItem(menu,  "Select Scenario"));
  //  select_scenario_button->sig_click().connect(boost::bind(&MenuManager::display_scenario_menu, this));
  // menu->add_item(select_scenario_button);

  std::auto_ptr<ButtonMenuItem> options_button(new ButtonMenuItem(menu,  "Options"));
  options_button->sig_click().connect(boost::bind(&MenuManager::display_option_menu, this));
  menu->add_item(options_button.release());

  std::auto_ptr<ButtonMenuItem> debug_button(new ButtonMenuItem(menu,  "Debug"));
  debug_button->sig_click().connect(boost::bind(&MenuManager::display_debug_menu, this));
  menu->add_item(debug_button.release());

  std::auto_ptr<ButtonMenuItem> credits_button(new ButtonMenuItem(menu,  "Credits"));
  credits_button->sig_click().connect(boost::bind(&MenuManager::display_credits, this));
  menu->add_item(credits_button.release());

  std::auto_ptr<ButtonMenuItem> help_button(new ButtonMenuItem(menu,  "Help"));
  help_button->sig_click().connect(boost::bind(&MenuManager::display_help, this));
  menu->add_item(help_button.release());

  std::auto_ptr<ButtonMenuItem> quit_button(new ButtonMenuItem(menu,  "Return to Title Screen"));
  quit_button->sig_click().connect(boost::bind(&MenuManager::menu_exit, this));
  menu->add_item(quit_button.release());
  // End: Option Menu

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release()); 
}

void
MenuManager::display_models_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-275, 100), Sizef(550, 376)),  // 378
                                                         "Select Model",
                                                         manager->get_root()));

  MenuComponent* menu = new MenuComponent(Rectf(), true, group.get());
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  std::vector<std::string> models;
  models.push_back("models/characters/bob/bob.wsprite");
  models.push_back("models/characters/jane/jane.wsprite");
  models.push_back("models/characters/monster/monster.wsprite");
  models.push_back("models/characters/powersuit/powersuit.wsprite");
  models.push_back("models/characters/sophie/sophie.wsprite");
  models.push_back("models/characters/spider/spider.wsprite");
  models.push_back("models/characters/vrdummy/vrdummy.wsprite");
  models.push_back("models/characters/yagor/yagor.wsprite");
  models.push_back("models/objects/grenade/grenade.wsprite");
  models.push_back("models/objects/pistol/pistol.wsprite");
  models.push_back("models/vehicles/shuttle/shuttle.wsprite");
  models.push_back("models/vehicles/train/train.wsprite"); 

  for(std::vector<std::string>::iterator i = models.begin(); i != models.end(); ++i)
    {
      std::auto_ptr<ButtonMenuItem> scenario_button(new ButtonMenuItem(menu,  *i));
      scenario_button->sig_click().connect(boost::bind(&MenuManager::menu_show_model, this, std::string(*i)));
      menu->add_item(scenario_button.release());
    }

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());  
}

void
MenuManager::display_particle_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-200, 300-170), Sizef(400, 340)), 
                                                         "Particle Systems",
                                                         manager->get_root()));

  MenuComponent* menu(new MenuComponent(Rectf(), true, group.get()));

  menu->set_font(Fonts::vera20);
  group->pack(menu);

  std::vector<std::string> scenarios;
  scenarios.push_back("particlesystems/fire.particles");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      std::auto_ptr<ButtonMenuItem> scenario_button(new ButtonMenuItem(menu, *i));
      scenario_button->sig_click().connect(boost::bind(&MenuManager::menu_show_particle_system, this, *i));
      menu->add_item(scenario_button.release());
    }

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_scenario_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-200, 300-170), Sizef(400, 340)), 
                                                         "Select Scenario",
                                                         manager->get_root()));

  MenuComponent* menu = new MenuComponent(Rectf(), true, group.get());
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
      std::auto_ptr<ButtonMenuItem> scenario_button(new ButtonMenuItem(menu,  *i));
      scenario_button->sig_click().connect(boost::bind(&MenuManager::menu_start_scenario, this, *i));
      menu->add_item(scenario_button.release());
    }

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_debug_menu()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-250, 300-170), Sizef(500, 340)), 
                                                         "Debug",
                                                         manager->get_root()));

  // Begin Menu
  MenuComponent* menu = new MenuComponent(Rectf(), true, group.get());
  group->pack(menu);

  menu->set_font(Fonts::vera20);

  Color amb = Sector::current()->get_ambient_light();

  std::auto_ptr<SliderMenuItem> r_ambient_light_item(new SliderMenuItem(menu,  "Ambient Light (Red)", int(amb.r*100), 0, 100, 10));
  r_ambient_light_item->sig_change().connect(boost::bind(&MenuManager::menu_ambient_light, this, _1, 0));
  menu->add_item(r_ambient_light_item.release());

  std::auto_ptr<SliderMenuItem> g_ambient_light_item(new SliderMenuItem(menu,  "Ambient Light (Green)", int(amb.g*100), 0, 100, 10));
  g_ambient_light_item->sig_change().connect(boost::bind(&MenuManager::menu_ambient_light, this, _1, 1));
  menu->add_item(g_ambient_light_item.release());

  std::auto_ptr<SliderMenuItem> b_ambient_light_item(new SliderMenuItem(menu,  "Ambient Light (Blue)", int(amb.b*100), 0, 100, 10));
  b_ambient_light_item->sig_change().connect(boost::bind(&MenuManager::menu_ambient_light, this, _1, 2));
  menu->add_item(b_ambient_light_item.release());

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release()); 
}

void
MenuManager::display_help()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-250, 300-200), Sizef(500, 400)), 
                                                         "Help",
                                                         manager->get_root()));

  TextView* text = new TextView(Rectf(), group.get());
  group->pack(text);

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

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_credits()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(Rectf(Vector2f(400-250, 300-200), Sizef(500, 400)), 
                                                         "Credits",
                                                         manager->get_root()));

  TextView* text = new TextView(Rectf(), group.get());
  group->pack(text);

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

  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}
  
// Callbacks

void
MenuManager::menu_show_model(std::string model)
{
  std::auto_ptr<Sprite3DView> sprite3dview(new Sprite3DView());

  sprite3dview->set_model(model);

  // Launching Sprite3DView instead of game
  screen_manager.push_screen(sprite3dview.release());
  screen_manager.clear_overlay();
}

void
MenuManager::menu_show_particle_system(std::string file)
{
  std::auto_ptr<ParticleViewer> particle_viewer(new ParticleViewer());
  particle_viewer->load(file);

  screen_manager.push_screen(particle_viewer.release());
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

void
MenuManager::menu_show_geometry_test()
{
  screen_manager.push_screen(new GeometryTest());
  screen_manager.clear_overlay();  
}

void
MenuManager::menu_show_armature_test()
{
  screen_manager.push_screen(new ArmatureTest());
  screen_manager.clear_overlay();  
}

void
MenuManager::menu_show_navigation_test()
{
  screen_manager.push_screen(new NavigationTest());
  screen_manager.clear_overlay();  
}

void
MenuManager::menu_gamma(int i)
{
  float gamma = i / 100.0f;
  SDL_SetGamma(gamma, gamma, gamma);
}

void
MenuManager::menu_wiimote()
{
#ifdef HAVE_CWIID
  if (wiimote && !wiimote->is_connected())
    {
      // FIXME: This never appears on screen due to timeout
      console << "Trying to connect Wiimote: Press buttons 1 and 2 to connect" << std::endl;

      wiimote->connect();

      if (wiimote->is_connected())
        console << "Wiimote connected" << std::endl;
      else
        console << "Wiimote connection failed" << std::endl;
    }
  else
    {
      console << "Wiimote already is connected" << std::endl;
    }
#endif
}

void
MenuManager::menu_music_volume(int i)
{
  config.set_int("master-volume", i);
  sound_manager->set_listener_gain((i/100.0f));
}

/* EOF */
