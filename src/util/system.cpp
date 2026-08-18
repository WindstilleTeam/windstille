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

#include <fstream>
#include <vector>
#include <filesystem>

#include <stdexcept>
#include <sstream>
#include <stdlib.h>

#if defined(ANDROID) || defined(__ANDROID__)
#  include <SDL.h>
#  include <android/log.h>
#endif

std::string System::find_default_datadir()
{
#ifdef __EMSCRIPTEN__
  // Assets are preloaded at /data by the wasm shell / --preload-file.
  return "/data/";
#elif defined(ANDROID) || defined(__ANDROID__)
  // Prefer extracted tree under internal storage (see prepare_android_datadir).
  if (char const* internal = SDL_AndroidGetInternalStoragePath()) {
    std::string root = std::string(internal) + "/game-data/";
    return root;
  }
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

#if defined(ANDROID) || defined(__ANDROID__)

namespace {

std::vector<uint8_t> android_read_asset(char const* path)
{
  SDL_RWops* rw = SDL_RWFromFile(path, "rb");
  if (!rw) {
    return {};
  }
  Sint64 size = SDL_RWsize(rw);
  if (size < 0) {
    SDL_RWclose(rw);
    return {};
  }
  std::vector<uint8_t> buf(static_cast<size_t>(size));
  size_t got = SDL_RWread(rw, buf.data(), 1, static_cast<size_t>(size));
  SDL_RWclose(rw);
  if (got != static_cast<size_t>(size)) {
    return {};
  }
  return buf;
}

} // namespace

void
System::prepare_android_datadir()
{
  char const* internal = SDL_AndroidGetInternalStoragePath();
  if (!internal) {
    throw std::runtime_error("SDL_AndroidGetInternalStoragePath returned null");
  }

  std::filesystem::path root = std::filesystem::path(internal) / "game-data";
  std::filesystem::path marker = root / ".assets-extracted";

  if (!std::filesystem::exists(marker)) {
    __android_log_print(ANDROID_LOG_INFO, "windstille",
                        "Extracting APK assets to %s", root.c_str());
    std::filesystem::create_directories(root);

    auto index = android_read_asset("android-asset-index.txt");
    int extracted = 0;
    if (index.empty()) {
      __android_log_print(ANDROID_LOG_WARN, "windstille",
                          "android-asset-index.txt missing in APK assets");
    } else {
      std::string text(index.begin(), index.end());
      std::size_t pos = 0;
      while (pos < text.size()) {
        std::size_t end = text.find('\n', pos);
        if (end == std::string::npos) end = text.size();
        std::string rel = text.substr(pos, end - pos);
        if (!rel.empty() && rel.back() == '\r') rel.pop_back();
        pos = end + 1;
        if (rel.empty() || rel == "android-asset-index.txt") continue;

        auto bytes = android_read_asset(rel.c_str());
        if (bytes.empty()) {
          __android_log_print(ANDROID_LOG_WARN, "windstille",
                              "skip missing asset: %s", rel.c_str());
          continue;
        }
        std::filesystem::path out = root / rel;
        std::filesystem::create_directories(out.parent_path());
        std::ofstream ofs(out, std::ios::binary);
        ofs.write(reinterpret_cast<char const*>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
        if (ofs) {
          ++extracted;
        }
      }
    }

    if (extracted > 0) {
      std::ofstream(marker) << "1\n";
      __android_log_print(ANDROID_LOG_INFO, "windstille",
                          "Asset extract done (%d files)", extracted);
    } else {
      __android_log_print(ANDROID_LOG_ERROR, "windstille",
                          "Asset extract produced 0 files — will retry next launch");
    }
  }

  // Pathname expects trailing slash via set_datadir
  std::string datadir = root.string();
  if (!datadir.empty() && datadir.back() != '/') datadir += '/';
  // set later in main via Pathname::set_datadir — return path by env or just set here
  // We set via Pathname in main after this call; store in a static for find_default_datadir.
}

// Override find_default_datadir for Android to use extracted tree
// (definition already exists above — patch it)

#endif
