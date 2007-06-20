//  $Id: file_reader.hxx,v 1.4 2003/10/18 23:17:27 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_PINGUS_FILE_READER_HXX
#define HEADER_PINGUS_FILE_READER_HXX

#include <string>
#include <vector>
#include "sharedptr.hpp"

class Size;
class Color;
class Vector;
class Vector3;

class ResDescriptor;
class FileReaderImpl;

/** Interface to read name/value pairs out of some kind of file or
    structure */
class FileReader
{
public:
  static FileReader parse(const std::string& filename);

  FileReader(SharedPtr<FileReaderImpl> impl_);
  FileReader();

  /** Name of the current section, ie. in the case of
      <groundpiece><pos>...</groundpiece> it would be 'groundpiece' */
  std::string get_name() const;

  bool read_int    (const char* name, int&)           const;
  bool read_float  (const char* name, float&)         const;
  bool read_bool   (const char* name, bool&)          const;
  bool read_string (const char* name, std::string&)   const;
  bool read_vector3(const char* name, Vector3&)      const;
  bool read_vector (const char* name, Vector&)    const;
  bool read_size   (const char* name, Size&)          const;
  bool read_color  (const char* name, Color&)         const;
  bool read_section(const char* name, FileReader&)   const;
  FileReader read_section(const char* name)   const;

  template<class E, class T>
  bool read_enum  (const char* name, E& value, T enum2string) const
  {
    std::string str;
    if (read_string(name, str))
      {
        value = enum2string(str);
        return true;
      }

    return false;
  }

  bool get(const char* name, std::vector<int>&   v) const;
  bool get(const char* name, std::vector<float>& v) const;
  bool get(const char* name, std::vector<std::string>& v) const;

  bool get(const char* name, FileReader& v) { return read_section(name, v); }
  bool get(const char* name, int&   v) { return read_int(name, v); }
  bool get(const char* name, float& v) { return read_float(name, v); }
  bool get(const char* name, bool& v) { return read_bool(name, v); }
  bool get(const char* name, std::string& v) { return read_string(name, v); }
  bool get(const char* name, Vector3& v) { return read_vector3(name, v); }
  bool get(const char* name, Vector& v) { return read_vector(name, v); }
  bool get(const char* name, Size& v) { return read_size(name, v); }
  bool get(const char* name, Color& v) { return read_color(name, v); } 

  std::vector<std::string> get_section_names() const;
  std::vector<FileReader>  get_sections() const;

  void print_unused_warnings(const std::string& title);

private:
  SharedPtr<FileReaderImpl> impl;
};

#endif

/* EOF */
