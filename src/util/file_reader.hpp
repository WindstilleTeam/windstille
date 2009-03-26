/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_UTIL_FILE_READER_HPP
#define HEADER_WINDSTILLE_UTIL_FILE_READER_HPP

#include <string>
#include <vector>
#include "getter.hpp"
#include <boost/shared_ptr.hpp>

class FileReaderImpl;

/** Interface to read name/value pairs out of some kind of file or
    structure */
class FileReader
{
public:
  static FileReader parse(const std::string& filename);
  static FileReader parse(std::istream& stream, const std::string& filename);

  FileReader(boost::shared_ptr<FileReaderImpl> impl_);
  FileReader();

  /** Name of the current section */
  std::string get_name() const;

  std::vector<std::string> get_section_names() const;
  std::vector<FileReader>  get_sections() const;

  /** Generic getter function for non-standard types, see getter.hpp */
  template<typename T>
  bool get(const char* name, T& v) {
    return ::get(*this, name, v);
  }
 
  template<class E, class T>
  bool get(const char* name, E& value, T enum2string)
  {
    std::string str;
    if (get(name, str))
      {
        value = enum2string(str);
        return true;
      }

    return false;
  }

  // Primitive types
  // FIXME: Is there any nice way to not use two different names?
  bool read(const char* name, FileReader& v);

  bool read(const char* name, int&   v);
  bool read(const char* name, float& v);
  bool read(const char* name, bool& v);
  bool read(const char* name, std::string& v);

  bool read(const char* name, std::vector<bool>&   v);
  bool read(const char* name, std::vector<int>&   v);
  bool read(const char* name, std::vector<float>& v);
  bool read(const char* name, std::vector<std::string>& v);

private:
  boost::shared_ptr<FileReaderImpl> impl;
};

#endif

/* EOF */
