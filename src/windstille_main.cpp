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

#include "windstille.hpp"

#include <stdio.h>
#include <physfs.h>

#include "globals.hpp"
#include "screen.hpp"
#include "windstille_main.hpp"
#include "font/fonts.hpp"
#include "game_session.hpp"
#include "title_screen.hpp"
#include "sector.hpp"
#include "input/input_manager.hpp"
#include "sound/sound_manager.hpp"
#include "tile_factory.hpp"
#include "script_manager.hpp"
#include "tinygettext/gettext.hpp"
#include "config.hpp"
#include "util.hpp"
#include "font/ttf_font.hpp"
#include "display/display.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "sprite3d/manager.hpp"
#include "screen_manager.hpp"
#include "sprite3dview.hpp"
#include "sprite2dview.hpp"
#include "particle_viewer.hpp"
#include "sprite2d/manager.hpp"

WindstilleMain::WindstilleMain()
{
}

WindstilleMain::~WindstilleMain()
{
}

int 
WindstilleMain::main(int argc, char** argv)
{
  try {
    config.parse_args(argc, argv);

    init_physfs(argv[0]);
    init_sdl();

    dictionaryManager = new tinygettext::DictionaryManager();
    dictionaryManager->set_charset("iso8859-1");
    dictionaryManager->add_directory("locale");

    config.load();
    
    config.parse_args(argc, argv);

    init_modules();

    { // Fill controller_description with data
      controller_description.add_button("primary-button",   PRIMARY_BUTTON);
      controller_description.add_button("secondary-button", SECONDARY_BUTTON);
      controller_description.add_button("tertiary-button",  TERTIARY_BUTTON);

      controller_description.add_button("menu-up-button",  MENU_UP_BUTTON);
      controller_description.add_button("menu-down-button",  MENU_DOWN_BUTTON);

      controller_description.add_button("menu-left-button",  MENU_LEFT_BUTTON);
      controller_description.add_button("menu-right-button",  MENU_RIGHT_BUTTON);
  
      controller_description.add_button("view-center-button", VIEW_CENTER_BUTTON);

      controller_description.add_button("pda-button", PDA_BUTTON);
      controller_description.add_button("inventory-button", INVENTORY_BUTTON);

      controller_description.add_button("aim-button",       AIM_BUTTON);
      controller_description.add_button("pause-button",     PAUSE_BUTTON);

      controller_description.add_axis("x-axis", X_AXIS);
      controller_description.add_axis("y-axis", Y_AXIS);

      controller_description.add_axis("x2-axis", X2_AXIS);
      controller_description.add_axis("y2-axis", Y2_AXIS);

      controller_description.add_ball("mouse-motion-x", MOUSE_MOTION_X);
      controller_description.add_ball("mouse-motion-y", MOUSE_MOTION_Y);
    }

      {
        if (config.get<std::string>("controller-file").is_set())
          InputManager::init(config.get<std::string>("controller-file").get());
        else if (PHYSFS_exists("controller.cfg"))
          InputManager::init("controller.cfg");
        else
          InputManager::init("controller/keyboard.scm");
      }

    if (debug) std::cout << "Initialising TileFactory" << std::endl;
    TileFactory::init();
    
    if (debug) std::cout << "Starting file: '" << config.get_string("levelfile") << "'" 
                         << std::endl;
    
    std::string levelfile;
    if (config.get<std::string>("levelfile").is_set())
      {
        // FIXME: Looks a little hacky, doesn't it?
        std::string leveldir = dirname(config.get_string("levelfile"));
        PHYSFS_addToSearchPath(leveldir.c_str(), true);
        levelfile = basename(config.get_string("levelfile"));
        std::cout << "XXX: " << leveldir << std::endl;
      }

    if (sprite3dview)
      {
        Sprite3DView* sprite3dview = new Sprite3DView();

        if (!levelfile.empty())
          sprite3dview->set_model(levelfile);

        // Launching Sprite3DView instead of game
        screen_manager.push_screen(sprite3dview);
      }
    else if (sprite2dview)
      {
        Sprite2DView* sprite2dview = new Sprite2DView();

        if (!levelfile.empty())
          sprite2dview->set_sprite(levelfile);

        // Launching Sprite2DView instead of game
        screen_manager.push_screen(sprite2dview);
      }
    else if (particleview)
      {
        ParticleViewer* particle_viewer = new ParticleViewer();
        if (!levelfile.empty())
          particle_viewer->load(levelfile);
        screen_manager.push_screen(particle_viewer);
      }
    else
      {
        if (levelfile.empty())
          {
            //screen_manager.push_screen(new GameSession("levels/newformat2.wst"));
            screen_manager.push_screen(new TitleScreen());
          }
        else
          {
            screen_manager.push_screen(new GameSession(levelfile));
          }
      }
        
    screen_manager.run();
    
    TileFactory::deinit();
    InputManager::deinit();

    deinit_modules();

  } catch (std::exception& err) {
    std::cout << "std::exception: " << err.what() << std::endl;
  } catch (...) {
    std::cout << "Error catched something unknown?!" << std::endl;
  }

  config.save();

  delete dictionaryManager;
  dictionaryManager = 0;
  
  SDL_Quit();
  PHYSFS_deinit();

  return 0;
}

void
WindstilleMain::init_modules()
{
  if (debug) std::cout << "Initialising Freetype2" << std::endl;
    
  TTFFont::init();
  
  Display::init();

  if (debug) std::cout << "Initialising Fonts" << std::endl;
  Fonts::init(); 
  sound_manager = new SoundManager();
  sound_manager->set_listener_gain(config.get_int("master-volume")/100.0f);
  sound_manager->enable_sound(config.get_bool("sound"));
  sound_manager->enable_music(config.get_bool("music"));

  if (debug) std::cout << "Initialising ScriptManager" << std::endl;
  texture_manager  = new TextureManager();
  surface_manager  = new SurfaceManager();
  script_manager   = new ScriptManager();
  sprite2d_manager = new sprite2d::Manager();
  sprite3d_manager = new sprite3d::Manager();

  script_manager->run_script_file("scripts/windstille.nut");
  script_manager->run_script_file("scripts/init.nut");
}

void
WindstilleMain::deinit_modules()
{
  delete sprite3d_manager;
  sprite3d_manager = 0;

  delete sprite2d_manager;
  sprite2d_manager = 0;

  delete script_manager;
  script_manager = 0;

  delete surface_manager;
  surface_manager = 0;

  delete texture_manager;
  texture_manager = 0;
  
  delete sound_manager;
  sound_manager = 0;
  Fonts::deinit();

  TTFFont::deinit();
}

void
WindstilleMain::init_sdl()
{
#ifdef DEBUG
  // I wanna have usefull backtraces in debug mode
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0) {
#else
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
#endif
    std::stringstream msg;
    msg << "Couldn't initialize SDL: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  SDL_EnableUNICODE(1);
}

void
WindstilleMain::init_physfs(const char* argv0)
{
  if(!PHYSFS_init(argv0)) {
    std::stringstream msg;
    msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }

  // Initialize physfs (this is a slightly modified version of
  // PHYSFS_setSaneConfig
  const char* application = "windstille";
  const char* userdir = PHYSFS_getUserDir();
  const char* dirsep  = PHYSFS_getDirSeparator();
  char* writedir = new char[strlen(userdir) + strlen(application) + 2];

  // Set configuration directory
  sprintf(writedir, "%s.%s", userdir, application);
  if(!PHYSFS_setWriteDir(writedir)) {
    // try to create the directory
    char* mkdir = new char[strlen(application) + 2];
    sprintf(mkdir, ".%s", application);
    if(!PHYSFS_setWriteDir(userdir) || !PHYSFS_mkdir(mkdir)) {
      std::ostringstream msg;
      msg << "Failed creating configuration directory '"
          << writedir << "': " << PHYSFS_getLastError();
      delete[] writedir;
      delete[] mkdir;
      throw std::runtime_error(msg.str());
    }
    delete[] mkdir;

    if(!PHYSFS_setWriteDir(writedir)) {
      std::ostringstream msg;
      msg << "Failed to use configuration directory '"
          <<  writedir << "': " << PHYSFS_getLastError();
      delete[] writedir;
      throw std::runtime_error(msg.str());
    }
  }
  PHYSFS_addToSearchPath(writedir, 0);
  delete[] writedir;

  // Search for archives and add them to the search path
  const char* archiveExt = "zip";
  char** rc = PHYSFS_enumerateFiles("/");
  size_t extlen = strlen(archiveExt);

  for(char** i = rc; *i != 0; ++i) {
    size_t l = strlen(*i);
    if((l > extlen) && ((*i)[l - extlen - 1] == '.')) {
      const char* ext = (*i) + (l - extlen);
      if(strcasecmp(ext, archiveExt) == 0) {
        const char* d = PHYSFS_getRealDir(*i);
        char* str = new char[strlen(d) + strlen(dirsep) + l + 1];
        sprintf(str, "%s%s%s", d, dirsep, *i);
        PHYSFS_addToSearchPath(str, 1);
        delete[] str;
      }
    }
  }

  PHYSFS_freeList(rc);

  // when started from source dir...
  ::datadir = PHYSFS_getBaseDir();
  ::datadir += "data/";
  std::string testfname = ::datadir;
  testfname += "tiles.scm";
  bool sourcedir = false;
  FILE* f = fopen(testfname.c_str(), "r");
  if(f) {
    fclose(f);
    if(!PHYSFS_addToSearchPath(::datadir.c_str(), 1)) {
      std::cout << "Warning: Couldn't add '" << ::datadir
                << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    } else {
      sourcedir = true;
    }
  }

  if(!sourcedir) {
#if defined(APPDATADIR) || defined(ENABLE_BINRELOC)
#ifdef ENABLE_BINRELOC
    char* brdatadir = br_strcat(DATADIR, "/" PACKAGE_NAME);
    ::datadir = brdatadir;
    free(brdatadir);
#else
    ::datadir = APPDATADIR;
#endif
    if(!PHYSFS_addToSearchPath(::datadir.c_str(), 1)) {
      std::cout << "Couldn't add '" << ::datadir
        << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    }
#endif
  }

  // allow symbolic links
  PHYSFS_permitSymbolicLinks(1);

  //show search Path
  if (debug)
    {
      std::cout << "SearchPath:" << std::endl;
      char** search_path = PHYSFS_getSearchPath();
      for(char** i = search_path; *i != NULL; i++)
        std::cout << "  " << *i << std::endl;;
      PHYSFS_freeList(search_path);
    }
}

int main(int argc, char** argv)
{
  WindstilleMain main_app;

  main_app.main(argc, argv);

  return 0;
}

/* EOF */
