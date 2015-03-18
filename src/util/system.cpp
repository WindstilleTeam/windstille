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

#include "util/system.hpp"

#include <stdexcept>
#include <sstream>
#include <stdlib.h>
#ifdef HAVE_BINRELOC
#  include <binreloc.h>
#endif

std::string System::find_default_datadir()
{
#ifdef HAVE_BINRELOC
  std::string datadir = "data/";

  BrInitError error;
  if (!br_init(&error))
  {
    std::ostringstream out;
    out << "Error: Couldn't init binreloc: " << error;
    throw std::runtime_error(out.str());
  }
  else
  {
    char* c_prefix = br_find_exe_dir(NULL);
    if (!c_prefix)
    {
      throw std::runtime_error("Error: Couldn't find prefix");
    }
    else
    {
      datadir = c_prefix;
      free(c_prefix);
      datadir += "/data/";

      return datadir;
    }
  }
#elif _WIN32
  // TODO: do something with GetModuleFileName()
  return "data/";
#else
  return "data/";
#endif
}

std::string System::find_default_userdir()
{
#ifdef _WIN32
  char* appdata = getenv("APPDATA");
  if (!appdata)
  {
    throw std::runtime_error("APPDATA environment variable not set");
  }
  else
  {
    std::ostringstream out;
    out << appdata << "/windstille/";
    return out.str();
  }
#else
  char* xdg_config_home = getenv("XDG_CONFIG_HOME");
  if (xdg_config_home)
  {
    std::ostringstream out;
    out << xdg_config_home << "/windstille/";
    return out.str();
  }
  else
  {
    char* home = getenv("HOME");
    if (!home)
    {
      throw std::runtime_error("HOME environment variable not set");
    }
    else
    {
      std::ostringstream out;
      out << home << "/.config/windstille/";
      return out.str();
    }
  }
#endif
}
