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

#include "util/system.hpp"

#include <stdexcept>
#include <sstream>
#include <stdlib.h>

#if defined(ANDROID) || defined(__ANDROID__)
#  include <SDL.h>
#endif

std::string System::find_default_datadir()
{
#ifdef __EMSCRIPTEN__
  // Assets are preloaded at /data by the wasm shell / --preload-file.
  return "/data/";
#elif defined(ANDROID) || defined(__ANDROID__)
  // Assets are read via Android AssetManager; empty prefix keeps relative
  // paths like "images/…" which the SDL/Android physfs layer resolves.
  return "";
#elif defined(_WIN32)
  // TODO: do something with GetModuleFileName()
  return "data/";
#else
  return "data/";
#endif
}

std::string System::find_default_userdir()
{
#ifdef __EMSCRIPTEN__
  // Writable path in the Emscripten FS. Optional IDBFS mount can be layered
  // later for persistence across sessions (see mk/wasm/shell.html).
  return "/windstille-user/";
#elif defined(ANDROID) || defined(__ANDROID__)
  // App-private internal storage (writable). Root paths are not creatable
  // on device and would make init throw with no visible UI.
  if (char const* internal = SDL_AndroidGetInternalStoragePath())
  {
    return std::string(internal) + "/windstille/";
  }
  throw std::runtime_error("SDL_AndroidGetInternalStoragePath returned null");
#elif defined(_WIN32)
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
