/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "SDL.h"

#include "windstille.hpp"

#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>
#include <physfs.h>

#include "config.hpp"
#include "display/display.hpp"
#include "display/opengl_state.hpp"
#include "display/opengl_window.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "engine/script_manager.hpp"
#include "engine/sector.hpp"
#include "font/fonts.hpp"
#include "font/ttf_font.hpp"
#include "globals.hpp"
#include "input/input_manager.hpp"
#include "screen/game_session.hpp"
#include "screen/particle_viewer.hpp"
#include "screen/screen.hpp"
#include "screen/screen_manager.hpp"
#include "screen/sprite2dview.hpp"
#include "screen/sprite3dview.hpp"
#include "screen/title_screen.hpp"
#include "sound/sound_manager.hpp"
#include "sprite2d/manager.hpp"
#include "sprite3d/manager.hpp"
#include "tile/tile_factory.hpp"
#include "util/util.hpp"
#include "util/pathname.hpp"
#include "util/system.hpp"
#include "windstille_main.hpp"

WindstilleMain* WindstilleMain::current_ = 0;

WindstilleMain::WindstilleMain()
{
  current_ = this;
}

WindstilleMain::~WindstilleMain()
{
  current_ = 0;
}

int 
WindstilleMain::main(int argc, char** argv)
{
  try 
  {
    Pathname::set_datadir(System::find_default_datadir());
    Pathname::set_userdir(System::find_default_userdir());

    config.parse_args(argc, argv);

    init_physfs(argv[0]);
    init_sdl();

    config.load();
    
    config.parse_args(argc, argv);

    {
      m_window.reset(new OpenGLWindow());
        
      TTFFont::init(); 

      Fonts::init(); 
      new SoundManager();
  
      if (debug) std::cout << "Initialising ScriptManager" << std::endl;
      texture_manager  = new TextureManager();
      new SurfaceManager();
      new ScriptManager();
      sprite2d_manager = new SpriteManager();
      sprite3d_manager = new sprite3d::Manager();

      init_modules();
    
      run();
    }

    config.save();
    
    deinit_modules();
    PHYSFS_deinit();
  } 
  catch (std::exception& err)
  {
    std::cout << "std::exception: " << err.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Error catched something unknown?!" << std::endl;
  }

  return 0;
}

void
WindstilleMain::run()
{
  if (debug) 
    std::cout << "Starting file: '" << config.get_string("levelfile") << "'" << std::endl;
    
  if (config.get<std::string>("levelfile").is_set())
  {
    // FIXME: Looks a little hacky, doesn't it?
    std::string leveldir = dirname(config.get_string("levelfile"));
    PHYSFS_addToSearchPath(leveldir.c_str(), true);
    std::string levelfile = basename(config.get_string("levelfile"));

    // FIXME: file-type "detection" is pretty basic, only works
    // with s-expr and nothing else
    std::string file_type = FileReader::parse(levelfile).get_name();

    if (file_type == "sprite3d") // FIXME: sprite3d isn't actually a sexpr file
    {
      std::auto_ptr<Sprite3DView> sprite3dview(new Sprite3DView());

      if (!levelfile.empty())
        sprite3dview->set_model(levelfile);

      // Launching Sprite3DView instead of game
      screen_manager.push_screen(sprite3dview.release());
    }
    else if (file_type == "sprite") // FIXME: PNG are sprites too
    {
      std::auto_ptr<Sprite2DView> sprite2dview(new Sprite2DView());

      if (!levelfile.empty())
        sprite2dview->set_sprite(levelfile);

      // Launching Sprite2DView instead of game
      screen_manager.push_screen(sprite2dview.release());
    }
    else if (file_type == "particle-systems")
    {
      ParticleViewer* particle_viewer = new ParticleViewer();
      if (!levelfile.empty())
        particle_viewer->load(levelfile);
      screen_manager.push_screen(particle_viewer);
    }
    else if (file_type == "windstille-sector")
    {
      screen_manager.push_screen(new GameSession(levelfile));
    }
    else
    {
      throw std::runtime_error("Unknown filetype '" + file_type + "'");
    }
  }
  else
  {
    screen_manager.push_screen(new TitleScreen());
  }
  
  screen_manager.run();
}

void
WindstilleMain::set_fullscreen(bool fullscreen)
{ 
  m_window->set_fullscreen(fullscreen);
}

void
WindstilleMain::init_modules()
{
  if (debug) std::cout << "Initialising Fonts" << std::endl;
  SoundManager::current()->set_master_volume(config.get_int("master-volume")/100.0f);
  SoundManager::current()->enable_sound(config.get_bool("sound"));
  SoundManager::current()->enable_music(config.get_bool("music"));


  ScriptManager::current()->run_script_file("scripts/windstille.nut", true);

  { // Fill controller_description with data
    
    // DPad
    controller_description.add_button("menu-up-button",    MENU_UP_BUTTON);
    controller_description.add_button("menu-down-button",  MENU_DOWN_BUTTON);
    controller_description.add_button("menu-left-button",  MENU_LEFT_BUTTON);
    controller_description.add_button("menu-right-button", MENU_RIGHT_BUTTON);

    // Face Button
    controller_description.add_button("primary-button",    PRIMARY_BUTTON);
    controller_description.add_button("secondary-button",  SECONDARY_BUTTON);
    controller_description.add_button("tertiary-button",   TERTIARY_BUTTON);
    controller_description.add_button("quaternary-button", QUATERNARY_BUTTON);

    // Stick Buttons
    controller_description.add_button("left-stick-button",  LEFT_STICK_BUTTON);
    controller_description.add_button("right-stick-button", RIGHT_STICK_BUTTON);

    // Shoulder Button
    controller_description.add_button("left-shoulder-button",  LEFT_SHOULDER_BUTTON);
    controller_description.add_button("right-shoulder-button", RIGHT_SHOULDER_BUTTON);

    // Back/Start
    controller_description.add_button("select-button",    SELECT_BUTTON);
    controller_description.add_button("start-button",     START_BUTTON);
    controller_description.add_button("debug-button",     DEBUG_BUTTON);

    controller_description.add_axis("left-trigger-axis",  LEFT_TRIGGER_AXIS);
    controller_description.add_axis("right-trigger-axis", RIGHT_TRIGGER_AXIS);

    controller_description.add_axis("x-axis", X_AXIS);
    controller_description.add_axis("y-axis", Y_AXIS);

    controller_description.add_axis("x2-axis", X2_AXIS);
    controller_description.add_axis("y2-axis", Y2_AXIS);

    controller_description.add_ball("mouse-motion-x", MOUSE_MOTION_X);
    controller_description.add_ball("mouse-motion-y", MOUSE_MOTION_Y);
  }
    
  {
    InputManager::init();
      
    if (config.get<std::string>("primary-controller-file").is_set())
      InputManager::load(config.get<std::string>("primary-controller-file").get());
    else
      InputManager::load("controller/keyboard.scm");

    if (config.get<std::string>("secondary-controller-file").is_set())
      InputManager::load(config.get<std::string>("secondary-controller-file").get());
  }

  if (debug) std::cout << "Initialising TileFactory" << std::endl;
  TileFactory::init();
}

void
WindstilleMain::deinit_modules()
{
  TileFactory::deinit();
  InputManager::deinit();

  delete sprite3d_manager;
  sprite3d_manager = 0;

  delete sprite2d_manager;
  sprite2d_manager = 0;

  delete ScriptManager::current();
  
  delete SurfaceManager::current();

  delete texture_manager;
  texture_manager = 0;
  
  delete SoundManager::current();
  Fonts::deinit();

  TTFFont::deinit();
}

void
WindstilleMain::init_sdl()
{
#ifdef DEBUG
  // I wanna have usefull backtraces in debug mode
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE;
#else
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif

  if (SDL_Init(flags) < 0)
    {
      std::stringstream msg;
      msg << "Couldn't initialize SDL: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }

  SDL_EnableUNICODE(1);
}

void
WindstilleMain::init_physfs(const char* argv0)
{
  if (!PHYSFS_init(argv0))
  {
    std::stringstream msg;
    msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    boost::filesystem::create_directory(Pathname::get_userdir());
    boost::filesystem::create_directory(Pathname("screenshots", Pathname::kUserPath).get_sys_path());

    PHYSFS_setWriteDir(Pathname::get_userdir().c_str());
    PHYSFS_addToSearchPath(Pathname::get_userdir().c_str(), 0);
    PHYSFS_addToSearchPath(Pathname::get_datadir().c_str(), 0);

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);

    if (debug)
    { //show search Path
      std::cout << "userdir: " << Pathname::get_userdir() << std::endl;
      std::cout << "datadir: " << Pathname::get_datadir() << std::endl;

      std::cout << "SearchPath:" << std::endl;
      char** search_path = PHYSFS_getSearchPath();
      for(char** i = search_path; *i != NULL; i++)
        std::cout << "  " << *i << std::endl;;
      PHYSFS_freeList(search_path);
    }
  }
}

int main(int argc, char** argv)
{
  return WindstilleMain().main(argc, argv);
}

/* EOF */
