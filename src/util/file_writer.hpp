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

#ifndef HEADER_WINDSTILLE_UTIL_FILE_WRITER_HPP
#define HEADER_WINDSTILLE_UTIL_FILE_WRITER_HPP

#include <ostream>
#include <string>

class Color;
class Vector2f;

class FileWriter
{
private:
  std::ostream& out;
  int indent_count;

  void indent();

public:
  FileWriter(std::ostream& out);
  ~FileWriter();

  FileWriter& write_raw(const std::string& value);

  FileWriter& start_section(const std::string& name);
  FileWriter& end_section();

  FileWriter& write(const std::string& name, bool value);
  FileWriter& write(const std::string& name, int value);
  FileWriter& write(const std::string& name, uint32_t value);
  FileWriter& write(const std::string& name, float value);
  FileWriter& write(const std::string& name, const std::string& value);
  FileWriter& write(const std::string& name, const char* value);
  FileWriter& write(const std::string& name, const Color& value);
  FileWriter& write(const std::string& name, const Vector2f& value);

private:
  FileWriter(const FileWriter&);
  FileWriter& operator=(const FileWriter&);
};

#endif

/* EOF */
