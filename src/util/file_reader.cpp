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

#include "lisp/parser.hpp"
#include "sexpr_file_reader.hpp"
#include "file_reader.hpp"
#include "file_reader_impl.hpp"

FileReader
FileReader::parse(std::istream& stream, const std::string& filename)
{
  lisp::Lisp* root = lisp::Parser::parse(stream, filename);
  if (!root)
    {
      std::ostringstream msg;
      msg << "'" << filename << "': file not found";
      throw std::runtime_error(msg.str());
    }
  else if (root && root->get_type() == lisp::Lisp::TYPE_LIST && root->get_list_size() >= 1)
    {
      return SExprFileReader(root, root->get_list_elem(0));
    }
  else
    {
      std::ostringstream msg;
      msg << "'" << filename << "': not a valid sexpr file";
      throw std::runtime_error(msg.str());
    }  
}

FileReader
FileReader::parse(const std::string& filename)
{
  lisp::Lisp* root = lisp::Parser::parse(filename);
  if (!root)
    {
      std::ostringstream msg;
      msg << "'" << filename << "': file not found";
      throw std::runtime_error(msg.str());
    }
  else if (root && root->get_type() == lisp::Lisp::TYPE_LIST && root->get_list_size() >= 1)
    {
      return SExprFileReader(root, root->get_list_elem(0));
    }
  else
    {
      std::ostringstream msg;
      msg << "'" << filename << "': not a valid sexpr file";
      throw std::runtime_error(msg.str());
    }
}

FileReader::FileReader(boost::shared_ptr<FileReaderImpl> impl_)
  : impl(impl_)
{
}

FileReader::FileReader()
{
}

std::string
FileReader::get_name() const
{
  if (impl.get())
    return impl->get_name();
  else
    return "";
}

bool
FileReader::read(const char* name, int& value) const
{
  if (impl.get())
    return impl->read_int(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, float& value) const
{
  if (impl.get())
    return impl->read_float(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, bool& value) const
{
  if (impl.get())
    return impl->read_bool(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, std::string& value) const
{
  if (impl.get())
    return impl->read_string(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, std::vector<int>& value) const
{
  if (impl.get())
    return impl->get(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, std::vector<bool>&   value) const
{
  if (impl.get())
    return impl->get(name, value);
  else
    return false;  
}

bool
FileReader::read(const char* name, std::vector<std::string>& value) const
{
  if (impl.get())
    return impl->get(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, std::vector<float>& value) const
{
  if (impl.get())
    return impl->get(name, value);
  else
    return false;
}

bool
FileReader::read(const char* name, FileReader& reader) const
{
  if (impl.get())
    return impl->read_section(name, reader);
  else
    return false;
}

std::vector<std::string>
FileReader::get_section_names() const
{
  if (impl.get())
    return impl->get_section_names();
  else
    return std::vector<std::string>();
}

std::vector<FileReader>
FileReader::get_sections() const
{
  if (impl.get())
    return impl->get_sections();
  else
    return std::vector<FileReader>();
}

/* EOF */
