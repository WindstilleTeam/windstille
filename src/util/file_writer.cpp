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

#include <sstream>

#include "display/color.hpp"
#include "math/vector2f.hpp"
#include "util/file_writer.hpp"

FileWriter::FileWriter(std::ostream& out_)
  : out(out_),
    indent_count(0)
{
}

FileWriter::~FileWriter()
{
}

void
FileWriter::indent()
{
  out << '\n';
  for(int i = 0; i < indent_count; ++i)
    out << "  ";
}

FileWriter&
FileWriter::write_raw(const std::string& value)
{
  out << value;
  return *this;
}

FileWriter&
FileWriter::start_section(const std::string& name)
{
  indent();
  out << "(" << name;
  indent_count += 1;
  return *this;
}

FileWriter&
FileWriter::end_section()
{
  out << ")";
  indent_count -= 1;
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, bool value)
{
  indent();
  out << "(" << name << " " << (value?"#t":"#f") << ")";
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, int value)
{
  indent();
  out << "(" << name << " " << value << ")";
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, float value)
{
  indent();
  out << "(" << name << " " << value << ")";
  return *this;
}

static
std::string escape_string(const std::string& in)
{
  std::ostringstream str;
  for(std::string::const_iterator i = in.begin(); i != in.end(); ++i)
  {
    if (*i == '"')
      str << "\\\"";
    else if (*i == '\\')
      str << "\\\\";
    else
      str << *i;
  }
  return str.str();
}

FileWriter&
FileWriter::write(const std::string& name, const char* value)
{
  indent();
  out << "(" << name << " \"" << escape_string(value) << "\")";
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, const std::string& value)
{
  indent();
  out << "(" << name << " \"" << escape_string(value) << "\")";
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, const Color& value)
{
  indent();
  out << "(" << name << " " << value.r << " " << value.g << " " << value.b << " " << value.a << ")";
  return *this;
}

FileWriter&
FileWriter::write(const std::string& name, const Vector2f& value)
{
  indent();
  out << "(" << name << " " << value.x << " " << value.y << ")";
  return *this;
}

/* EOF */
