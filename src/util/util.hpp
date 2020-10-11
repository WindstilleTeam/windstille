/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_UTIL_UTIL_HPP
#define HEADER_WINDSTILLE_UTIL_UTIL_HPP

#include <sstream>
#include <stdint.h>

std::string dirname(const std::string& filename);
std::string basename(const std::string& filename);

/** Loads a file from \a filename and places its content in \a str
    FIXME: doesn't use PhysFS */
void file_to_string(const std::string& filename, std::string& str);

template<class T>
std::string to_string(const T& data)
{
  std::ostringstream os;
  os << data;
  return os.str();
}

float       read_float(std::istream& in);
uint16_t    read_uint16_t(std::istream& in);
uint32_t    read_uint32_t(std::istream& in);
std::string read_string(std::istream& in, size_t size);

inline bool is_big_endian()
{
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};

  return bint.c[0] == 1;
}

inline bool is_little_endian()
{
  return !is_big_endian();
}

uint16_t    byte_swap16(uint16_t v);
uint32_t    byte_swap32(uint32_t v);

#endif

/* EOF */
