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

#ifndef HEADER_WINDSTILLE_UTIL_DIRECTORY_HPP
#define HEADER_WINDSTILLE_UTIL_DIRECTORY_HPP

#include <vector>
#include <string>

class Pathname;

class Directory
{
public:
  typedef std::vector<Pathname> List;

  /** Read the given directory and return the file entries */
  static List read(const Pathname& pathname);

  /**
   *  Read the given directory and return the file entries, only
   *  entries having the suffix \a suffix will be included in the
   *  result
   */
  static List read(const Pathname& pathname, const std::string& suffix);

private:
  Directory(const Directory&);
  Directory& operator=(const Directory&);
};

#endif

/* EOF */
