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

#ifndef HEADER_WINDSTILLE_GETTER_HPP
#define HEADER_WINDSTILLE_GETTER_HPP

#include "file_reader.hpp"

class FileReader;
class Quaternion;
class Color;
class Vector;
class Vector3;
class Matrix;

bool get(FileReader& reader, const char* name, Matrix& value);
bool get(FileReader& reader, const char* name, Quaternion& value);
bool get(FileReader& reader, const char* name, Color& value);
bool get(FileReader& reader, const char* name, Vector&  value);
bool get(FileReader& reader, const char* name, Vector3& value);

// Primitive types
bool get(FileReader& reader, const char* name, FileReader& v);

bool get(FileReader& reader, const char* name, int&   v);
bool get(FileReader& reader, const char* name, float& v);
bool get(FileReader& reader, const char* name, bool& v);
bool get(FileReader& reader, const char* name, std::string& v);

bool get(FileReader& reader, const char* name, std::vector<bool>&   v);
bool get(FileReader& reader, const char* name, std::vector<int>&   v); 
bool get(FileReader& reader, const char* name, std::vector<float>& v);
bool get(FileReader& reader, const char* name, std::vector<std::string>& v);

#endif

/* EOF */
