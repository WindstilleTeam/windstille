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

#include <sstream>
#include <filesystem>

#include <surf/save.hpp>
#include <wstdisplay/font/ttf_font_manager.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstgui/frame_hud.hpp>
#include <wstgui/screen_manager.hpp>
#include <wstgui/style.hpp>
#include <wstinput/input_manager.hpp>
#include <wstsystem/system.hpp>

#include "app/app.hpp"
#include "app/config.hpp"
#include "app/console.hpp"
#include "app/controller_def.hpp"
#include "app/windstille_main.hpp"
#include "engine/script_manager.hpp"
#include "font/fonts.hpp"
#include "hud/controller_help_window.hpp"
#include "screen/game_session.hpp"
#include "screen/input_configurator.hpp"
#include "screen/particle_viewer.hpp"
#include "screen/sprite2dview.hpp"
#include "screen/sprite3dview.hpp"
#include "screen/title_screen.hpp"
#include "sound_manager.hpp"
#include "sprite3d/manager.hpp"
#include "tile/tile_factory.hpp"
#include "util/system.hpp"

namespace windstille {

using namespace wstdisplay;

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

    wstsys::System system;

    config.load();

    config.parse_args(argc, argv);

    {
      auto window = system.create_window("Windstille",
                                         geom::isize(config.get_int("screen-width"), config.get_int("screen-height")));
                                         //geom::isize(config.get_int("aspect-width"), config.get_int("aspect-height")),
                                         //config.get_bool("fullscreen"), config.get_int("anti-aliasing"));
      wstdisplay::TTFFontManager    ttffont_manager;
      Fonts             fonts(ttffont_manager);
      Console           console;
      SoundManager      sound_manager;
      wstdisplay::TextureManager    texture_manager;
      wstdisplay::SurfaceManager    surface_manager;
      SpriteManager     sprite_manager(surface_manager);
      sprite3d::Manager sprite3d_manager;
      ScriptManager     script_manager;
      wstinput::ControllerDescription controller_description = get_windstille_controller_description();
      wstinput::InputManagerSDL   input_manager(controller_description);
      wstgui::ScreenManager screen_manager(system, *window, input_manager);
      TileFactory       tile_factory = TileFactory(Pathname("tiles.scm"));
      wstgui::Style     style(fonts.vera20.get());

      screen_manager.sig_update().connect([&](float dt){
        sound_manager.update(dt);
      });

      g_app.m_sound_manager = &sound_manager;
      g_app.m_input_manager = &input_manager;
      g_app.m_texture_manager = &texture_manager;
      g_app.m_surface_manager = &surface_manager;
      g_app.m_sprite_manager = &sprite_manager;
      g_app.m_sprite3d_manager = &sprite3d_manager;
      g_app.m_window = window.get();
      g_app.m_ttffont_manager = &ttffont_manager;
      g_app.m_screen_manager = &screen_manager;
      g_app.m_fonts = &fonts;
      g_app.m_style = &style;

      window->set_icon(Pathname("icon.png"));
      texture_manager.set_fallback(Pathname("images/404.png"));

      init_modules();

      screen_manager.bind_key(SDLK_F1, []{
        if (!Console::current()->is_active()) {
          Console::current()->activate();
        } else {
          // FIXME: g_app.input().on_event(event); // FIXME: Why?
        }
      });

      screen_manager.bind_key(SDLK_F6, []{
        SDL_SetRelativeMouseMode(SDL_FALSE);
      });

      screen_manager.bind_key(SDLK_F7, []{
        SDL_SetRelativeMouseMode(SDL_TRUE);
      });

      bool show_controller_help_window = false;
      std::unique_ptr<ControllerHelpWindow> controller_help_window = std::make_unique<ControllerHelpWindow>();
      screen_manager.bind_key(SDLK_F8, [&screen_manager, &controller_help_window, &show_controller_help_window]{
        if (!show_controller_help_window) {
          screen_manager.add_hud(controller_help_window.get());
          show_controller_help_window = true;
        } else {
          screen_manager.remove_hud(controller_help_window.get());
          show_controller_help_window = false;
        }
      });

      screen_manager.bind_key(SDLK_F9, [&screen_manager]{
        screen_manager.push_overlay(std::make_unique<InputConfigurator>());
      });

      std::unique_ptr<wstgui::FrameHud> frame_hud = std::make_unique<wstgui::FrameHud>(style);
      screen_manager.bind_key(SDLK_F10, [&screen_manager, &frame_hud]{
        bool const show_fps = !config.get_bool("show-fps");
        config.set_bool("show-fps", show_fps);
        if (show_fps) {
          screen_manager.add_hud(frame_hud.get());
        } else {
          screen_manager.remove_hud(frame_hud.get());
        }
      });

      screen_manager.bind_key(SDLK_F11, []{
        config.set_bool("fullscreen", !config.get_bool("fullscreen"));
        g_app.window().set_mode(config.get_bool("fullscreen") ?
                                OpenGLWindow::Mode::Fullscreen :
                                OpenGLWindow::Mode::Window);
      });

      screen_manager.bind_key(SDLK_F12, []{
        // FIXME: Replace this with Physfs stuff
        int count = 0;
        Pathname filename;
        do {
          filename = Pathname(fmt::format("screenshots/windstille{:04d}.png", count), Pathname::kUserPath);
          count += 1;
        } while(filename.exists());

        surf::save(g_app.window().screenshot(), filename);
        ConsoleLog << "Writing screenshot to: '" << filename << "'" << std::endl;
      });

      run();

      config.save();

      g_app.m_sound_manager = nullptr;
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
      std::unique_ptr<Sprite3DView> sprite3dview(new Sprite3DView());
      sprite3dview->set_model(filename);
      g_app.screen().push_screen(std::move(sprite3dview));
    }
    else if (file_type == "sprite" || file_type == "png" || file_type == "jpg")
    {
      std::unique_ptr<Sprite2DView> sprite2dview(new Sprite2DView());
      sprite2dview->set_sprite(filename);
      g_app.screen().push_screen(std::move(sprite2dview));
    }
    else if (file_type == "particles")
    {
      auto particle_viewer = std::make_unique<ParticleViewer>();
      particle_viewer->load(filename);
      g_app.screen().push_screen(std::move(particle_viewer));
    }
    else if (file_type == "wst")
    {
      g_app.screen().push_screen(std::make_unique<GameSession>(filename));
    }
    else
    {
      throw std::runtime_error("Unknown filetype '" + file_type + "'");
    }
  }
  else
  {
    g_app.screen().push_screen(std::make_unique<TitleScreen>());
  }

  g_app.screen().run();
}

void
WindstilleMain::init_modules()
{
  g_app.sound().set_master_gain(static_cast<float>(config.get_int("master-volume"))/100.0f);
  g_app.sound().enable_sound(config.get_bool("sound"));
  g_app.sound().enable_music(config.get_bool("music"));

  ScriptManager::current()->run_script_file(Pathname("scripts/windstille.nut"), true);

  {
    // Hardcoded defaults
    g_app.input().bindings().bind_keyboard_button(ENTER_BUTTON, SDL_SCANCODE_RETURN);
    g_app.input().bindings().bind_keyboard_button(ESCAPE_BUTTON, SDL_SCANCODE_ESCAPE);
    g_app.input().bindings().bind_keyboard_button(MENU_LEFT_BUTTON, SDL_SCANCODE_LEFT);
    g_app.input().bindings().bind_keyboard_button(MENU_RIGHT_BUTTON, SDL_SCANCODE_RIGHT);
    g_app.input().bindings().bind_keyboard_button(MENU_UP_BUTTON, SDL_SCANCODE_UP);
    g_app.input().bindings().bind_keyboard_button(MENU_DOWN_BUTTON, SDL_SCANCODE_DOWN);

    // User configuration
    if (config.get<std::string>("primary-controller-file").is_set()) {
      g_app.input().load(Pathname(config.get<std::string>("primary-controller-file").get(),
                                  Pathname::kSysPath).get_sys_path());
    } else {
      g_app.input().load(Pathname("controller/keyboard.scm").get_sys_path());
    }

    if (config.get<std::string>("secondary-controller-file").is_set()) {
      g_app.input().load(Pathname(config.get<std::string>("secondary-controller-file").get(),
                                  Pathname::kSysPath).get_sys_path());
    }
  }
}

} // namespace windstille

int main(int argc, char** argv)
{
  return windstille::WindstilleMain().main(argc, argv);
}

/* EOF */
