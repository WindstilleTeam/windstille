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
#include "getter.hpp"
#include "sharedptr.hpp"

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

  void print_unused_warnings(const std::string& title) const;

private:
  SharedPtr<FileReaderImpl> impl;
};

#endif

/* EOF */
