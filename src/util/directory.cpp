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

#include "util/directory.hpp"

#include <boost/filesystem.hpp>

#include "util/util.hpp"
#include "util/pathname.hpp"

Directory::List
Directory::read(const Pathname& pathname)
{
  std::vector<Pathname> entries;
  
  for (boost::filesystem::directory_iterator it(pathname.get_sys_path());
       it != boost::filesystem::directory_iterator();
       ++it)
  {
    // FIXME: Would make sense to try to keep the Pathname::type
    // intact, instead of converting everything to system path
    entries.push_back(Pathname(it->path().string(), Pathname::kSysPath));
  }

  return entries;
}

Directory::List
Directory::read(const Pathname& pathname, const std::string& suffix)
{
  std::vector<Pathname> entries;
  
  for (boost::filesystem::directory_iterator it(pathname.get_sys_path());
       it != boost::filesystem::directory_iterator();
       ++it)
  {
    // FIXME: Would make sense to try to keep the Pathname::type
    // intact, instead of converting everything to system path
    const std::string& filename = it->path().string();
    if (has_suffix(filename, suffix))
    {
      entries.push_back(Pathname(filename, Pathname::kSysPath));
    }
  }

  return entries;  
}

#ifdef __TEST__
#include <iostream>

int main(int argc, char** argv)
{
  if (argc == 2)
  {
    Pathname directory_name(argv[1], Pathname::kSysPath);
    std::cout << "Directory: " << directory_name << std::endl;
    const Directory::List& directory = Directory::read(directory_name);
    for(Directory::List::const_iterator p = directory.begin(); p != directory.end(); ++p)
    {
      std::cout << "  Entry: " << *p << std::endl;
    }
  }
  else if (argc == 3)
  {
    Pathname directory_name(argv[1], Pathname::kSysPath);
    std::cout << "Directory: " << directory_name << std::endl;
    const Directory::List& directory = Directory::read(directory_name, argv[2]);
    for(Directory::List::const_iterator p = directory.begin(); p != directory.end(); ++p)
    {
      std::cout << "  Entry: " << *p << std::endl;
    }
  }
  else
  {
    std::cout << "Usage: " << argv[0] << " DIRECTORY [PATTERN]" << std::endl;
  }
}

#endif

/* EOF */
