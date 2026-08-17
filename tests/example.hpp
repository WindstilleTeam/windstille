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

#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#ifndef SCRIPTING_API
#include <string>

#include <squirrel.h>

#define __suspend
#define __custom(x)
#endif

namespace example {

class Example
{
public:
  Example();
  ~Example();

  void do_foobar();
  void do_bazbaz();
};

void do_foobar();
void do_add(int a, int b);

bool do_return_bool();
int do_return_int();
float do_return_float();
std::string do_return_string();

bool do_return_bool_id(bool value);
int do_return_int_id(int value);
float do_return_float_id(float value);
std::string do_return_string_id(std::string const& value);

void do_suspend() __suspend;

/*
  The argument to __custom() acts as type signature for the custom
  function. For possible values, see:

  http://www.squirrel-lang.org/squirreldoc/reference/api/object_creation_and_handling.html#c.sq_setparamscheck
 */
SQInteger do_custom(HSQUIRRELVM vm) __custom(".isi");

} // namespace example

#endif
