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

#include "util/util.hpp"

#include <fstream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdexcept>
#include <vector>

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return "";

  return filename.substr(0, p+1);
}

std::string basename(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return filename;

  return filename.substr(p+1, filename.size()-p);
}

void file_to_string(const std::string& filename, std::string& str)
{
  std::ifstream in(filename.c_str());
  while (true)
  {
    char c = static_cast<char>(in.get());
    if (!in.good())
      break;
    str += c;
  }
}

bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

std::string tolowercase(const std::string& str)
{
  std::string out;
  for(std::string::const_iterator i = str.begin();  i != str.end(); ++i)
    out += static_cast<char>(tolower(*i));
  return out;
}

float read_float(std::istream& in)
{
  union {
    float    float_v;
    uint32_t raw_v;
  } result;

  if (!in.read(reinterpret_cast<char*>(&result.raw_v), sizeof(result.raw_v)))
  {
    std::ostringstream msg;
    msg << "Problem reading float value: " << strerror(errno);
    throw std::runtime_error(msg.str());
  }
  else
  {
    if (is_big_endian())
    {
      result.raw_v = byte_swap32(result.raw_v);
      return result.float_v;
    }
    else
    {
      return result.float_v;
    }
  }
}

uint16_t read_uint16_t(std::istream& in)
{
  uint16_t result;

  if (!in.read(reinterpret_cast<char*>(&result), sizeof(result)))
  {
    std::ostringstream msg;
    msg << "Problem reading uint16 value: " << strerror(errno);
    throw std::runtime_error(msg.str());
  }
  else
  {
    if (is_big_endian())
    {
      return byte_swap16(result);
    }
    else
    {
      return result;
    }
  }
}

uint32_t read_uint32_t(std::istream& in)
{
  uint32_t result;

  if (!in.read(reinterpret_cast<char*>(&result), sizeof(result)))
  {
    std::ostringstream msg;
    msg << "Problem reading uint32 value: " << strerror(errno);
    throw std::runtime_error(msg.str());
  }
  else
  {
    if (is_big_endian())
    {
      return byte_swap32(result);
    }
    else
    {
      return result;
    }
  }
}

std::string read_string(std::istream& in, size_t size)
{
  std::vector<char> buffer(size + 1);

  if (!in.read(buffer.data(), size))
  {
    std::ostringstream msg;
    msg << "Problem reading string value: " << strerror(errno);
    throw std::runtime_error(msg.str());
  }
  else
  {
    buffer[size] = '\0';
    return std::string(buffer.data());
  }
}

uint16_t
byte_swap16(uint16_t v)
{
#ifdef __GNUC__
  return static_cast<uint16_t>(v >> 8 | v << 8);
#else
  return _byteswap_ushort(v);
#endif
}

uint32_t
byte_swap32(uint32_t v)
{
#if defined(__APPLE__)
  // FIXME: there are probably faster ways, also inline all this stuff
  return
    (v & 0xff000000) >> 24 |
    (v & 0x00ff0000) >>  8 |
    (v & 0x0000ff00) <<  8 |
    (v & 0x000000ff) << 24;
#elif defined(__GNUC__)
  return __builtin_bswap32(v);
#else
  return _byteswap_ulong(v);
#endif
}

/* EOF */
