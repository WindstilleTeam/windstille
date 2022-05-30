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

#include <gtkmm/stock.h>

#include "editor/about_window.hpp"

namespace windstille {


AboutWindow::AboutWindow()
{
  //set_logo(0);
  set_license("Windstille - A Sci-Fi Action-Adventure Game\n"
              "Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>\n"
              "\n"
              "This program is free software: you can redistribute it and/or modify\n"
              "it under the terms of the GNU General Public License as published by\n"
              "the Free Software Foundation, either version 3 of the License, or\n"
              "(at your option) any later version.\n"
              "\n"
              "This program is distributed in the hope that it will be useful,\n"
              "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
              "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
              "GNU General Public License for more details.\n"
              "\n"
              "You should have received a copy of the GNU General Public License\n"
              "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
  set_name("Windstille Editor");
  set_version("0.0.1");
  set_comments("An Editor for Windstille");

  std::vector<Glib::ustring> authors;
  authors.push_back("Ingo Ruhnke <grumbel@gmail.com>");
  set_authors(authors);
}

AboutWindow::~AboutWindow()
{
}


} // namespace windstille

/* EOF */
