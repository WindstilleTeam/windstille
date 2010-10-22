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

#include <sstream>
#include <boost/filesystem.hpp>

#include "app/config.hpp"
#include "app/console.hpp"
#include "display/opengl_window.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "engine/script_manager.hpp"
#include "font/fonts.hpp"
#include "input/input_manager_sdl.hpp"
#include "screen/game_session.hpp"
#include "screen/particle_viewer.hpp"
#include "screen/screen_manager.hpp"
#include "screen/sprite2dview.hpp"
#include "screen/sprite3dview.hpp"
#include "screen/title_screen.hpp"
#include "sound/sound_manager.hpp"
#include "sprite3d/manager.hpp"
#include "tile/tile_factory.hpp"
#include "util/system.hpp"
#include "app/windstille_main.hpp"

WindstilleMain::WindstilleMain()
{
}

WindstilleMain::~WindstilleMain()
{
}

int 
WindstilleMain::main(int argc, char** argv)
{
  try 
  {
    Pathname::set_datadir(System::find_default_datadir());
    Pathname::set_userdir(System::find_default_userdir());

    config.parse_args(argc, argv);

    init_sdl();

    config.load();
    
    config.parse_args(argc, argv);

    {
      OpenGLWindow      window("Windstille",
                               Size(config.get_int("screen-width"), config.get_int("screen-height")),
                               Size(config.get_int("aspect-width"), config.get_int("aspect-height")),
                               config.get_bool("fullscreen"), config.get_int("anti-aliasing"));
      TTFFontManager    ttffont_manager;
      Fonts             fonts;
      Console           console;
      SoundManager      sound_manager;
      TextureManager    texture_manager;
      SurfaceManager    surface_manager;
      SpriteManager     sprite_manager;
      sprite3d::Manager sprite3d_manager;
      ScriptManager     script_manager;
      WindstilleControllerDescription controller_description;
      InputManagerSDL   input_manager(controller_description);
      ScreenManager     screen_manager;
      TileFactory       tile_factory = TileFactory(Pathname("tiles.scm"));

      init_modules();
    
      run();

      config.save();
    }
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
  if (config.get<std::string>("levelfile").is_set())
  {
    Pathname filename(config.get_string("levelfile"), Pathname::kSysPath);

    const std::string file_type = filename.get_extension();

    if (file_type == "wsprite")
    {
      std::auto_ptr<Sprite3DView> sprite3dview(new Sprite3DView());
      sprite3dview->set_model(filename);
      ScreenManager::current()->push_screen(sprite3dview.release());
    }
    else if (file_type == "sprite" || file_type == "png" || file_type == "jpg")
    {
      std::auto_ptr<Sprite2DView> sprite2dview(new Sprite2DView());
      sprite2dview->set_sprite(filename);
      ScreenManager::current()->push_screen(sprite2dview.release());
    }
    else if (file_type == "particles")
    {
      ParticleViewer* particle_viewer = new ParticleViewer();
      particle_viewer->load(filename);
      ScreenManager::current()->push_screen(particle_viewer);
    }
    else if (file_type == "wst")
    {
      ScreenManager::current()->push_screen(new GameSession(filename));
    }
    else
    {
      throw std::runtime_error("Unknown filetype '" + file_type + "'");
    }
  }
  else
  {
    ScreenManager::current()->push_screen(new TitleScreen());
  }
  
  ScreenManager::current()->run();
}

void
WindstilleMain::init_modules()
{
  SoundManager::current()->set_gain(static_cast<float>(config.get_int("master-volume"))/100.0f);
  SoundManager::current()->enable_sound(config.get_bool("sound"));
  SoundManager::current()->enable_music(config.get_bool("music"));

  ScriptManager::current()->run_script_file(Pathname("scripts/windstille.nut"), true);
    
  {     
    if (config.get<std::string>("primary-controller-file").is_set())
      InputManagerSDL::current()->load(Pathname(config.get<std::string>("primary-controller-file").get(), 
                                                Pathname::kSysPath));
    else
      InputManagerSDL::current()->load(Pathname("controller/keyboard.scm"));

    if (config.get<std::string>("secondary-controller-file").is_set())
      InputManagerSDL::current()->load(Pathname(config.get<std::string>("secondary-controller-file").get(),
                                                Pathname::kSysPath));
  }
}

void
WindstilleMain::init_sdl()
{
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;

  if (SDL_Init(flags) < 0)
  {
    std::stringstream msg;
    msg << "Couldn't initialize SDL: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    atexit(SDL_Quit);
    SDL_EnableUNICODE(1);
  }
}

int main(int argc, char** argv)
{
  return WindstilleMain().main(argc, argv);
}

/* EOF */
