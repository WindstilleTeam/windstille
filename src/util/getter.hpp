/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_UTIL_GETTER_HPP
#define HEADER_WINDSTILLE_UTIL_GETTER_HPP

#include "math/matrix.hpp"
#include "math/vector3.hpp"
#include "math/vector2f.hpp"
#include "math/quaternion.hpp"
#include "util/file_reader.hpp"

class FileReader;
class Color;

bool get(const FileReader& reader, const char* name, Matrix& value);
bool get(const FileReader& reader, const char* name, Quaternion& value);
bool get(const FileReader& reader, const char* name, Color& value);
bool get(const FileReader& reader, const char* name, Vector2f& value);
bool get(const FileReader& reader, const char* name, Vector3& value);

// Primitive types
bool get(const FileReader& reader, const char* name, FileReader& v);

bool get(const FileReader& reader, const char* name, int&   v);
bool get(const FileReader& reader, const char* name, float& v);
bool get(const FileReader& reader, const char* name, bool& v);
bool get(const FileReader& reader, const char* name, std::string& v);

bool get(const FileReader& reader, const char* name, std::vector<bool>&   v);
bool get(const FileReader& reader, const char* name, std::vector<int>&   v); 
bool get(const FileReader& reader, const char* name, std::vector<float>& v);
bool get(const FileReader& reader, const char* name, std::vector<std::string>& v);

#endif

/* EOF */
