// miniswig - Language binding generator for Squirrel
// Copyright (C) 2023 Ingo Ruhnke <grumbel@gmail.com>
//               2006 Matthias Braun <matze@braunis.de>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cstdio>
#include <fstream>
#include <stdexcept>

#include <squirrel.h>

inline std::string load_file(std::string const& filename)
{
  std::ifstream fin(filename, std::ios::ate);

  if (!fin) {
    throw std::runtime_error("failed to open file: " + filename);
  }

  std::streamsize const size = fin.tellg();
  fin.seekg(0, std::ios::beg);

  std::string content(size, '\0');
  if (!fin.read(content.data(), content.size())) {
    throw std::runtime_error("failed to read from file: " + filename);
  }

  return content;
}

inline void print_squirrel_stack(HSQUIRRELVM v)
{
  printf("--------------------------------------------------------------\n");
  SQInteger count = sq_gettop(v);
  for (int i = 1; i <= count; ++i) {
    printf("%d: ",i);
    switch (sq_gettype(v, i))
    {
      case OT_NULL:
        printf("null");
        break;
      case OT_INTEGER: {
        SQInteger val;
        sq_getinteger(v, i, &val);
        printf("integer (%d)", static_cast<int>(val));
        break;
      }
      case OT_FLOAT: {
        SQFloat val;
        sq_getfloat(v, i, &val);
        printf("float (%f)", static_cast<double>(val));
        break;
      }
      case OT_STRING: {
        SQChar const* val;
        sq_getstring(v, i, &val);
        printf("string (%s)", val);
        break;
      }
      case OT_TABLE:
        printf("table");
        break;
      case OT_ARRAY:
        printf("array");
        break;
      case OT_USERDATA:
        printf("userdata");
        break;
      case OT_CLOSURE:
        printf("closure(function)");
        break;
      case OT_NATIVECLOSURE:
        printf("native closure(C function)");
        break;
      case OT_GENERATOR:
        printf("generator");
        break;
      case OT_USERPOINTER:
        printf("userpointer");
        break;
      case OT_THREAD:
        printf("thread");
        break;
      case OT_CLASS:
        printf("class");
        break;
      case OT_INSTANCE:
        printf("instance");
        break;
      case OT_WEAKREF:
        printf("weakref");
        break;
      default:
        printf("unknown?!?");
        break;
    }
    printf("\n");
  }
  printf("--------------------------------------------------------------\n");
}

/* EOF */
