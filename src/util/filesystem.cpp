// surf - Software surface library
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <fstream>
#include <future>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <utime.h>
#include <sstream>
#include <algorithm>
#include <logmich/log.hpp>

#include "util/filesystem.hpp"

namespace surf {

std::string
Filesystem::find_exe(const std::string& name)
{
  char* path_c = getenv("PATH");
  if (path_c)
  {
    const char* delim = ":";
    char* path = strdup(path_c);
    char* state;

    for(char* p = strtok_r(path, delim, &state); p != nullptr; p = strtok_r(nullptr, delim, &state))
    {
      std::ostringstream fullpath;
      fullpath << p << "/" << name;
      if (access(fullpath.str().c_str(), X_OK) == 0)
      {
        free(path);
        return fullpath.str();
      }
    }

    free(path);

    throw std::runtime_error("Filesystem::find_exe(): Couldn't find " + name + " in PATH");
  }
  else
  {
    throw std::runtime_error("Filesystem::find_exe(): Couldn't get PATH environment variable");
  }
}

std::string
Filesystem::get_extension(std::filesystem::path const& path)
{
  std::string const& pathname = path.string();
  // FIXME: should take '/' into account and only check the actual
  // filename, instead of the whole pathname
  std::string::size_type p = pathname.rfind('.');
  std::string extension = pathname.substr(p+1, pathname.size() - p);

  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  if (extension == "gz" ||
      extension == "bz2")
  {
    p = pathname.rfind('.', p-1);
    extension = pathname.substr(p+1, pathname.size() - p);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
  }
  else
  {
    return extension;
  }
}

std::string
Filesystem::get_magic(std::filesystem::path const& filename)
{
  char buf[512];
  std::ifstream in(filename, std::ios::binary);
  if (!in)
  {
    int err = errno;
    throw std::runtime_error(filename.string() + ": couldn't open file: " + strerror(err));
  }
  else
  {
    if (in.read(buf, sizeof(buf)).bad())
    {
      throw std::runtime_error(filename.string() + ": failed to read " + std::to_string( sizeof(buf)) + " bytes");
    }
    else
    {
      return std::string(buf, static_cast<size_t>(in.gcount()));
    }
  }
}

void
Filesystem::readlines_from_file(const std::string& pathname, std::vector<std::string>& lst)
{
  std::ifstream in(pathname.c_str());

  if (!in)
  {
    throw std::runtime_error("Filesystem::readlines_from_file(): Couldn't open file: " + pathname);
  }
  else
  {
    std::string line;
    while(std::getline(in, line))
    {
      lst.push_back(line);
    }
    in.close();
  }
}

} // namespace surf

/* EOF */
