// miniswig - Language binding generator for Squirrel
// Copyright (C) 2023 Ingo Ruhnke <grumbel@gmail.com>
//
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

#include "example.hpp"

#include <iostream>

#include "util.hpp"
#include "example_wrap.hpp"

namespace example {

Example::Example()
{
  std::cout << "Example::Example()\n";
}

Example::~Example()
{
  std::cout << "Example::~Example()\n";
}

void
Example::do_foobar()
{
  std::cout << "Example::do_foobar()\n";
}

void
Example::do_bazbaz()
{
  std::cout << "Example::do_bazbaz()\n";
}

void do_foobar()
{
  std::cout << "do_foobar()\n";
}

void do_add(int a, int b)
{
  std::cout << "do_bazbaz(" << a << ", " << b << ") -> " << a + b << "\n";
}

bool do_return_bool()
{
  return true;
}

int do_return_int()
{
  return 45;
}

float do_return_float()
{
  return 0.125f;
}

std::string do_return_string()
{
  return "Hello World";
}

bool do_return_bool_id(bool value)
{
  return value;
}

int do_return_int_id(int value)
{
  return value;
}

float do_return_float_id(float value)
{
  return value;
}

std::string do_return_string_id(std::string const& value)
{
  return value;
}

void do_suspend()
{
  std::cout << "do_suspend()\n";
}

SQInteger do_custom(HSQUIRRELVM vm)
{
  std::cout << "do_custom(\n";

  // See: http://squirrel-lang.org/squirreldoc/reference/embedding/creating_a_c_function.html
  SQInteger const nargs = sq_gettop(vm);
  for(SQInteger n = 1; n <= nargs; ++n)
  {
    std::cout << "  stack " << n << ": ";
    switch(sq_gettype(vm, n))
    {
      case OT_NULL:
        std::cout << "null";
        break;
      case OT_INTEGER:
        std::cout << "integer";
        break;
      case OT_FLOAT:
        std::cout << "float";
        break;
      case OT_STRING:
        std::cout << "string";
        break;
      case OT_TABLE:
        std::cout << "table";
        break;
      case OT_ARRAY:
        std::cout << "array";
        break;
      case OT_USERDATA:
        std::cout << "userdata";
        break;
      case OT_CLOSURE:
        std::cout << "closure(function)";
        break;
      case OT_NATIVECLOSURE:
        std::cout << "native closure(C function)";
        break;
      case OT_GENERATOR:
        std::cout << "generator";
        break;
      case OT_USERPOINTER:
        std::cout << "userpointer";
        break;
      case OT_CLASS:
        std::cout << "class";
        break;
      case OT_INSTANCE:
        std::cout << "instance";
        break;
      case OT_WEAKREF:
        std::cout << "weak reference";
        break;
      default:
        return sq_throwerror(vm, "invalid param"); //throws an exception
    }
    std::cout << '\n';
  }
  std::cout << "  )\n";

  sq_pushinteger(vm, nargs); // push the number of arguments as return value
  return 1;
}

} // namespace example

/* EOF */
