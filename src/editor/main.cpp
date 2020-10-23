/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include <gdkmm/glcontext.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/main.h>

#include <argparser.hpp>
#include <logmich/log.hpp>

#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include "editor/app.hpp"
#include "editor/editor_window.hpp"
#include "editor/main.hpp"
#include "sprite2d/manager.hpp"
#include "sprite3d/manager.hpp"
#include "util/system.hpp"

int
WindstilleEditor::main(int argc, char** argv)
{
  try
  {
    std::string datadir;
    std::vector<std::string> rest_args;

    argparser::ArgParser argp;
    argp.add_usage(argv[0], "[LEVELFILE]")
      .add_text("Windstille Level Editor");

    argp.add_group()
      .add_option('h', "help", "", "Print this help")
      .add_option('d', "datadir", "DIR", "Fetch game data from DIR")
      .add_option('D', "debug", "", "Print debug level messages");

    for(auto const& opt : argp.parse_args(argc, argv))
    {
      switch (opt.key)
      {
        case 'D':
          logmich::g_logger.set_log_level(logmich::LogLevel::TRACE);
          break;

        case 'd':
          datadir = opt.argument;
          break;

        case 'h':
          argp.print_help();
          exit(EXIT_SUCCESS);
          break;

        case argparser::ArgumentType::REST:
          rest_args.push_back(opt.argument);
          break;
      }
    }

    Pathname::set_datadir(datadir.empty() ? System::find_default_datadir() : datadir);
    Pathname::set_userdir(System::find_default_userdir());

    Gtk::Main kit(&argc, &argv);

    TextureManager texture_manager;
    SurfaceManager surface_manager;
    SpriteManager  sprite_manager(surface_manager);
    sprite3d::Manager sprite3d_manager;

    g_app.m_texture_manager = &texture_manager;
    g_app.m_surface_manager = &surface_manager;
    g_app.m_sprite_manager = &sprite_manager;
    g_app.m_sprite3d_manager = &sprite3d_manager;

    Glib::RefPtr<Gtk::IconTheme> icon_theme = Gtk::IconTheme::get_default();
    icon_theme->append_search_path(Pathname("editor", Pathname::kDataPath).get_sys_path());


    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path(Pathname("style.css").get_sys_path());
    Glib::RefPtr<Gtk::StyleContext> style_context = Gtk::StyleContext::create();
    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
    style_context->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    EditorWindow window;
    window.show_all();
    window.show_minimap(false);

    if (rest_args.empty())
    {
      window.on_new();
    }
    else
    {
      for(std::vector<std::string>::iterator i = rest_args.begin(); i != rest_args.end(); ++i)
      {
        window.load_file(*i);
      }
    }

    Gtk::Main::run(window);
  }
  catch(Glib::Exception& err)
  {
    std::cout << "Glib::Exception: " << err.what() << std::endl;
  }
  catch(std::exception& err)
  {
    std::cout << "std::exception: " << err.what() << std::endl;
  }
  return 0;
}

int main(int argc, char** argv)
{
  WindstilleEditor app;
  return app.main(argc, argv);
}

/* EOF */
