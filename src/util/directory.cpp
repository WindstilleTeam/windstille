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

#include "util/directory.hpp"

#include <filesystem>

#include "util/pathname.hpp"

namespace windstille {

Directory::List
Directory::read(Pathname const& pathname)
{
  std::vector<Pathname> entries;

  for (std::filesystem::directory_iterator it(pathname.get_sys_path());
       it != std::filesystem::directory_iterator();
       ++it)
  {
    // FIXME: Would make sense to try to keep the Pathname::type
    // intact, instead of converting everything to system path
    entries.push_back(Pathname(it->path().string(), Pathname::kSysPath));
  }

  return entries;
}

Directory::List
Directory::read(Pathname const& pathname, std::string const& suffix)
{
  std::vector<Pathname> entries;

  for (std::filesystem::directory_iterator it(pathname.get_sys_path());
       it != std::filesystem::directory_iterator();
       ++it)
  {
    std::string const& filename = it->path().string();
    if (filename.ends_with(suffix))
    {
      entries.push_back(Pathname(pathname.get_raw_path() + it->path().filename().string(), pathname.get_type()));
    }
  }

  return entries;
}

} // namespace windstille

/* EOF */
