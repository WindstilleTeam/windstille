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
#include "app/windstille_main.hpp"
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
#include "gui/menu.hpp"
#include "app/windstille.hpp"
#include "menu_manager.hpp"

void
MenuManager::display_option_menu()
{
  gui::Menu menu("Options", create_centered_rect(500, 340));

  menu.add_slider("Master Volume",  config.get_int("master-volume"), 0, 100, 10,
                  boost::bind(&MenuManager::menu_master_volume, _1));

  menu.add_slider("Music Volume", config.get_int("music-volume"), 0, 100, 10,
                  boost::bind(&MenuManager::menu_music_volume, _1));

  menu.add_slider("SFX Volume", config.get_int("sfx-volume"), 0, 100, 10, 
                  boost::bind(&MenuManager::menu_sfx_volume, _1));

  menu.add_slider("Voice Volume", config.get_int("voice-volume"), 0, 100, 10, 
                  boost::bind(&MenuManager::menu_voice_volume, _1));
  
  menu.add_enum("Aspect Ratio", 0)
    .add_pair(0, "4:3")
    .add_pair(1, "5:4")
    .add_pair(2, "16:9")
    .add_pair(3, "16:10")
    .add_pair(4, "letterbox");
  
  menu.add_enum("Show FPS", config.get_bool("show-fps"), 
                boost::bind(&MenuManager::menu_show_fps, _1))
    .add_pair(0, "off")
    .add_pair(1, "on");
  
  menu.add_enum("Fullscreen", config.get_bool("fullscreen"),
                boost::bind(&MenuManager::menu_fullscreen, _1))
    .add_pair(0, "off")
    .add_pair(1, "on");

  menu.add_enum("Controller Debug", screen_manager.get_show_controller_debug(),
                boost::bind(&MenuManager::menu_controller_debug, _1))
    .add_pair(0, "off")
    .add_pair(1, "on");

  menu.add_enum("Difficulty", 1)
    .add_pair(0, "easy")
    .add_pair(1, "medium")
    .add_pair(2, "hard");

  menu.add_slider("Gamma",  100, 10, 200, 10, boost::bind(&MenuManager::menu_gamma, _1));

#ifdef HAVE_CWIID
  if (wiimote)
    menu.add_button("Try to Connect Wiimote", boost::bind(&MenuManager::menu_wiimote));
#endif

  menu.show();
}

void
MenuManager::display_main_menu()
{
  gui::Menu menu("", create_positioned_rect(Vector2f(400-20, 200), Sizef(250, 254)), false);

  menu.add_button("Select Scenario", boost::bind(&MenuManager::display_scenario_menu));
  menu.add_button("Navigation Test", boost::bind(&MenuManager::menu_show_navigation_test));
  if (0) menu.add_button("Armature Test", boost::bind(&MenuManager::menu_show_armature_test));
  menu.add_button("Geometry Test", boost::bind(&MenuManager::menu_show_geometry_test));
  menu.add_button("Model Viewer", boost::bind(&MenuManager::display_models_menu));
  menu.add_button("Particle Systems", boost::bind(&MenuManager::display_particle_menu));
  menu.add_button("Options", boost::bind(&MenuManager::display_option_menu));
  menu.add_button("Credits", boost::bind(&MenuManager::display_credits));
  menu.add_button("Help", boost::bind(&MenuManager::display_help));
  menu.add_button("Quit", boost::bind(&MenuManager::menu_quit));
    
  { // Construct Copyright box
    std::auto_ptr<gui::GroupComponent> text_group
      (new gui::GroupComponent(Rectf(Vector2f(Display::get_width()/2 - 390,
                                              Display::get_height() - 100),
                                     Size(800 - 20, 
                                          100 - 10)),
                               "",
                               menu.get_root()));

    std::auto_ptr<gui::TextView> text(new gui::TextView(text_group->get_child_rect(), text_group.get()));
    text->set_font(Fonts::vera12);
    text->set_text("Windstille " WINDSTILLE_VERSION " - Copyright (C) 2009 Ingo Ruhnke &lt;grumbel@gmx.de&gt;\n"
                   "\n"
                   "This program is free software: you can redistribute it and/or modify "
                   "it under the terms of the GNU General Public License as published by "
                   "the Free Software Foundation, either version 3 of the License, or "
                   "(at your option) any later version.");
    text_group->pack(text.release());
    menu.get_root()->add_child(text_group.release());
  }

  menu.show();
}

void
MenuManager::display_pause_menu()
{
  gui::Menu menu("Pause Menu", create_centered_rect(400, 300));

  menu.add_button("Resume",  boost::bind(&MenuManager::menu_continue));
  if (Sector::current())
    {
      menu.add_button("Debug", boost::bind(&MenuManager::display_debug_menu));
      //menu.add_button("Select Scenario", boost::bind(&MenuManager::display_scenario_menu));
    }
  menu.add_button("Options", boost::bind(&MenuManager::display_option_menu));
  menu.add_button("Credits", boost::bind(&MenuManager::display_credits));
  menu.add_button("Help", boost::bind(&MenuManager::display_help));
  menu.add_button("Return to Title Screen", boost::bind(&MenuManager::menu_exit));
  
  menu.show();
}

void
MenuManager::display_models_menu()
{
  gui::Menu menu("Select Model", create_centered_rect(550, 376));
  
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
      menu.add_button(*i, boost::bind(&MenuManager::menu_show_model, std::string(*i)));
    }
  
  menu.show();
}

void
MenuManager::display_particle_menu()
{
  gui::Menu menu("Particle Systems", create_centered_rect(400, 340));

  std::vector<std::string> scenarios;
  scenarios.push_back("particlesystems/fire.particles");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      menu.add_button(*i, boost::bind(&MenuManager::menu_show_particle_system, *i));
    }

  menu.show();
}

void
MenuManager::display_scenario_menu()
{
  gui::Menu menu("Select Scenario", create_centered_rect(500, 340));

  std::vector<std::string> scenarios;
  scenarios.push_back("sectors/apartment/apartment.wst");
  scenarios.push_back("sectors/bluestone/bluestone.wst");
  scenarios.push_back("sectors/forest/forest.wst");
  scenarios.push_back("sectors/industrial/industrial.wst");
  scenarios.push_back("sectors/intro/intro.wst");
  scenarios.push_back("sectors/newformat2/newformat2.wst");
  scenarios.push_back("sectors/virtualreality/virtualreality.wst");
  
  for(std::vector<std::string>::iterator i = scenarios.begin(); i != scenarios.end(); ++i)
    {
      menu.add_button(*i, boost::bind(&MenuManager::menu_start_scenario, *i));
    }

  menu.show();
}

void
MenuManager::display_debug_menu()
{
  gui::Menu menu("Debug", create_centered_rect(500, 340));

  Color amb = Sector::current()->get_ambient_light();

  menu.add_slider("Ambient Light (Red)", int(amb.r*100), 0, 100, 10, 
                  boost::bind(&MenuManager::menu_ambient_light, _1, 0));

  menu.add_slider("Ambient Light (Green)", int(amb.g*100), 0, 100, 10, 
                  boost::bind(&MenuManager::menu_ambient_light, _1, 1));

  menu.add_slider("Ambient Light (Blue)", int(amb.b*100), 0, 100, 10, 
                  boost::bind(&MenuManager::menu_ambient_light, _1, 2));

  menu.show();
}

void
MenuManager::display_help()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(create_centered_rect(500, 400), 
                                                         "Help",
                                                         manager->get_root()));

  std::auto_ptr<TextView> text(new TextView(group->get_child_rect(), 
                                            group.get()));

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
                 "\n"
                 "Windstille Informations\n"
                 "=======================\n"
                 "\n"
                 "Windstille is a classic 2d jump'n shoot game in which the player will\n"
                 "be placed in a foreign alien world and has to find its way back into\n"
                 "safety. Windstille is currently available for GNU/Linux and Windows.\n"
                 "Its webpage at which one might find new versions, source code,\n"
                 "artworks and more informations is located at:\n"
                 "\n"
                 " * http://windstille.berlios.de/\n"
                 "\n"
                 "The latest development version can be obtained by:\n"
                 "\n"
                 " $ svn co svn://svn.berlios.de/windstille/trunk/windstille\n"
                 "\n"
                 "For informations on how to compile and run Windstille see the file\n"
                 "INSTALL. Windstille makes use of OpenGL so you will need working\n"
                 "hardware 3d support.\n"
                 "\n"
                 "Windstille is covered under the GNU GPL, which means that you can copy\n"
                 "and even modify it pretty much as you like, as long as you keep the\n"
                 "copyright headers in place and distribute the source too if you\n"
                 "distribute binaries, see the file COPYING for details.\n"
                 "\n"
                 "# EOF #\n"
                 );
  text->set_active(true);
  
  group->pack(text.release());
  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

void
MenuManager::display_credits()
{
  using namespace gui;
  std::auto_ptr<GUIManager> manager(new GUIManager());

  std::auto_ptr<GroupComponent> group(new GroupComponent(create_centered_rect(500, 400), 
                                                         "Credits",
                                                         manager->get_root()));

  std::auto_ptr<TextView> text(new TextView(group->get_child_rect(), group.get()));

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

  group->pack(text.release());
  manager->get_root()->add_child(group.release());
  screen_manager.push_overlay(manager.release());
}

Rectf
MenuManager::create_positioned_rect(const Vector2f& pos_, const Sizef& size)
{
  Vector2f pos = pos_;

  // move pos to center of rect
  pos.x += size.width  / 2.0f;
  pos.y += size.height / 2.0f;

  // scale the coordinates
  pos.x = pos.x * Display::get_width() / 800;
  pos.y = pos.y * Display::get_height() / 600;

  // move pos back to top/left
  pos.x -= size.width  / 2.0f;
  pos.y -= size.height / 2.0f;

  return Rectf(pos, size);
}

Rectf
MenuManager::create_centered_rect(float w, float h)
{
  return Rectf(Vector2f((Display::get_width()  - w)/2.0f,
                        (Display::get_height() - h)/2.0f),
               Sizef(w, h));
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
  WindstilleMain::current()->set_fullscreen(config.get_bool("fullscreen"));
}

void
MenuManager::menu_controller_debug(int i)
{
  screen_manager.show_controller_debug(i);
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
MenuManager::menu_master_volume(int i)
{
  config.set_int("master-volume", i);
  SoundManager::current()->set_master_volume((i/100.0f));
}

void
MenuManager::menu_music_volume(int i)
{
  config.set_int("music-volume", i);
  SoundManager::current()->set_music_volume((i/100.0f));
}

void
MenuManager::menu_voice_volume(int i)
{
  config.set_int("voice-volume", i);
  SoundManager::current()->set_voice_volume((i/100.0f));
}

void
MenuManager::menu_sfx_volume(int i)
{
  config.set_int("sfx-volume", i);
  SoundManager::current()->set_sfx_volume((i/100.0f));
}

/* EOF */
