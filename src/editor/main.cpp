/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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
#include <gtkmm/icontheme.h>
#include <gtkmm/main.h>
#include <gtkglmm.h>
#include <physfs.h>

#include "sprite2d/manager.hpp"
#include "display/texture_manager.hpp"
#include "editor_window.hpp"
#include "main.hpp"

int
WindstilleEditor::main(int argc, char** argv)
{
  try 
    {
      Gtk::Main kit(&argc, &argv);
      Gtk::GL::init(&argc, &argv);

      if (!PHYSFS_init(argv[0]))
        {
          std::ostringstream msg;
          msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
          throw std::runtime_error(msg.str());
        }
  
      PHYSFS_addToSearchPath("data/", 0);
      
      Glib::RefPtr<const Gdk::GL::Config> 
        glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                           //Gdk::GL::MODE_DEPTH);
                                           Gdk::GL::MODE_DOUBLE);
      if (!glconfig)
        {
          throw std::runtime_error("*** Cannot find any OpenGL-capable visual.");
        }

      sprite2d_manager = new SpriteManager();
      texture_manager  = new TextureManager();

      Glib::RefPtr<Gtk::IconTheme> icon_theme = Gtk::IconTheme::get_default();
      icon_theme->append_search_path("data/editor/");
      
      EditorWindow window(glconfig);
      window.show_all();
      window.show_minimap(false);

      if (argc == 1)
        {
          window.on_new();
        }
      else
        {
          for(int i = 1; i < argc; ++i)
            {
              window.load_file(argv[i]);
            }
        }
      
      Gtk::Main::run(window);
    }
  catch(std::exception& err)
    {
      std::cout << "Exception: " << err.what() << std::endl;
    }
  return 0;
}

int main(int argc, char** argv)
{
  WindstilleEditor app;
  return app.main(argc, argv);
}

/* EOF */
