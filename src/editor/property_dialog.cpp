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

#include "editor/property_dialog.hpp"

PropertyDialog::PropertyDialog(Properties& /*properties*/)
{
#if 0
  std::vector<std::string> props = properties.get_properties();
  for(std::vector<std::string>::iterator i = props.begin(); i != props.end(); ++i)
  {
    Property& prop = properties.get_property(*i);
    if (prop.get_type() == "int")
    {
      new Gtk::Label(*i);
      new Gtk::TextEntry();
    }
    else if (prop.get_type() == "float")
    {
    }
  }
#endif 
}

PropertyDialog::~PropertyDialog()
{
}
    

/* EOF */
