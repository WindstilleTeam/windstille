/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCRIPTING_UTIL_HPP
#define HEADER_WINDSTILLE_SCRIPTING_UTIL_HPP

#include <vector>
#include <squirrel.h>
#include <string>

namespace sexp {
class Value;
} // namespace sexp

namespace Scripting {

// Squirrel to Lisp
void        table_to_lisp(HSQUIRRELVM v, int table_idx, std::vector<sexp::Value>& entries);
void        sq_to_lisp(HSQUIRRELVM v, std::vector<sexp::Value>& entries);
std::string sq_to_lisp_string(std::string sq_str);
std::string squirrel2string(HSQUIRRELVM v, int i);

void print_squirrel_stack(HSQUIRRELVM v, const std::string& context = std::string());

} // namespace Scripting

#endif

/* EOF */
