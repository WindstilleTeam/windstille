/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_WINDSTILLE_SCRIPTING_UTIL_HPP
#define HEADER_WINDSTILLE_SCRIPTING_UTIL_HPP

#include <squirrel.h>
#include <string>

namespace lisp {
class Writer;
class Lisp;
};

namespace Scripting {

// Squirrel to Lisp
void        table_to_lisp(HSQUIRRELVM v, int table_idx, std::vector<lisp::Lisp*>& entries);
void        sq_to_lisp(HSQUIRRELVM v, std::vector<lisp::Lisp*>& entries);
std::string sq_to_lisp_string(std::string sq_str);
std::string squirrel2string(HSQUIRRELVM v, int i);

void print_squirrel_stack(HSQUIRRELVM v);


// serialisation
void save_squirrel_table(HSQUIRRELVM v, int table_idx, lisp::Writer& writer);
void load_squirrel_table(HSQUIRRELVM v, int table_idx, const lisp::Lisp* lisp);

void save_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file);
void load_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file);

} // namespace Scripting

#endif

/* EOF */
