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

#ifndef HEADER_WINDSTILLE_UTIL_PATHNAME_HPP
#define HEADER_WINDSTILLE_UTIL_PATHNAME_HPP

#include <filesystem>
#include <map>
#include <string>

namespace windstille {

/**
 *  Pathname wraps a path and a path type, thus allowing the easy
 *  creation of paths that point to the data directory, the users
 *  savegame directory or the native filesystem along with allowing
 *  ways to manipulate the location of either of those.
 *
 *  A Pathname like:
 *
 *    Pathname path("maps\\test.png", Pathname::kDataPath);
 *
 *  Would be automatically expanded to:
 *
 *    path.getSysPath() => "C:\\Program Files\\Gicomm2\\data\\maps\\test.png"
 */
class Pathname
{
public:
  typedef std::map<std::string, std::string> Overrides;

  static void set_datadir(std::string const& datadir);
  static void set_userdir(std::string const& userdir);

  static std::string get_datadir();
  static std::string get_userdir();

  /**
   *  setDatadirOverrides() allows you to specify mappings from
   *  pathnames to native filesystem that override the default, thus
   *  allowing you insert files into the virtual datadir without
   *  actually modifying the filesystem. This functionality is usefull
   *  for example for user modifications.
   */
  static void set_datadir_overrides(Overrides const& s_datadir_overrides);

  static void add_datadir_overrides(std::string const& path);
  static void clear_datadir_overrides();

private:
  static std::string s_datadir;
  static std::string s_userdir;

  static Overrides s_datadir_overrides;

public:
  enum PathType
  {
    kSysPath,  // A path that points directly to the filesystem
    kDataPath, // A path that is relative to the top of the data/ directory
    kUserPath, // A path that is relative to the top of the user directory
    kEmpty
  };

  Pathname()
    : m_path(),
      m_type(kEmpty)
  {}

  /** Create a Pathname object.
   *
   *  Create a path to the native filessytem:
   *  Pathname("C:\\User\\JUser\\Desktop\\test.png", Pathname::kSysPath)
   *
   *  Create a path to the data directory:
   *  Pathname("maps\\test.png", Pathname::kDataPath)
   *
   *  Create a path to the users savegame directory:
   *  Pathname("savegames\\save1.sav", Pathname::kUserPath)
   */
  explicit Pathname(std::string const& path, PathType type = kDataPath);

  bool empty() const { return m_type == kEmpty; }

  bool exists() const;

  Pathname get_dirname() const;
  Pathname get_basename() const;

  /** Appends the given path segment to Pathname, adding a '/'
   *  inbetween when needed */
  Pathname& append_path(std::string const& path);

  /** Appends text to the end of the path, not adding a '/' inbetween */
  Pathname& append_text(std::string const& path);

  /**
   *  Convert the given path and type to a string that points to the
   *  native filesystem path and which can be used in fopen(),
   *  fstream, etc.
   */
  std::string get_sys_path() const;

  /** Returns the relative unprocessed path */
  std::string get_raw_path() const { return m_path; }

  /** Returns the type of this Pathname */
  PathType    get_type()    const { return m_type; }

  /** Returns the files extension ('png', 'jpg', 'sprite', etc.) */
  std::string get_extension() const;

  bool operator<(Pathname const& rhs) const;

  bool operator==(Pathname const& rhs) const;
  bool operator!=(Pathname const& rhs) const;

  operator std::filesystem::path() const { return get_sys_path(); }

private:
  std::string m_path;
  PathType    m_type;
};

std::ostream& operator<<(std::ostream& s, Pathname const& path);

} // namespace windstille

#endif

/* EOF */
