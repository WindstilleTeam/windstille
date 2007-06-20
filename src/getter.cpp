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

#include "math/vector.hpp"
#include "math/vector3.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "color.hpp"
#include "file_reader.hpp"
#include "getter.hpp"

bool get(FileReader& reader, const char* name, Vector&  value)
{
  std::vector<float> floats;
  if (reader.get(name, floats) && floats.size() == 2) {
    value.x = floats[0];
    value.y = floats[1];
    return true;
  } else {
    return false;
  }  
}

bool get(FileReader& reader, const char* name, Vector3& value)
{
  std::vector<float> floats;
  if (reader.get(name, floats) && floats.size() == 3) {
    value.x = floats[0];
    value.y = floats[1];
    value.z = floats[2];
    return true;
  } else {
    return false;
  }
}

bool get(FileReader& reader, const char* name, Quaternion& value)
{
  std::vector<float> floats;
  if (reader.get(name, floats) && floats.size() == 4) {
    value.x = floats[0];
    value.y = floats[1];
    value.z = floats[2];
    value.w = floats[3];
    return true;
  } else {
    return false;
  }
}

bool get(FileReader& reader, const char* name, Color& value)
{
  std::vector<float> floats;
  if (reader.get(name, floats)) {
    if (floats.size() == 3)
      {
        value.r = floats[0];
        value.g = floats[1];
        value.b = floats[2];
        value.a = 1.0f;
        return true;
      }
    else if (floats.size() == 4)
      {
        value.r = floats[0];
        value.b = floats[1];
        value.g = floats[2];
        value.a = floats[3];
        return true;
      }
    else
      {
        return false;
      }
  } else {
    return false;
  } 
}

// Getters for primitive types
// FIXME: Ugly, maybe there is a better way then to have different read/get names for the function
bool get(FileReader& reader, const char* name, FileReader& v) { return reader.read(name, v); }

bool get(FileReader& reader, const char* name, int&   v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, float& v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, bool& v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, std::string& v) { return reader.read(name, v); }

bool get(FileReader& reader, const char* name, std::vector<bool>&   v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, std::vector<int>&   v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, std::vector<float>& v) { return reader.read(name, v); }
bool get(FileReader& reader, const char* name, std::vector<std::string>& v) { return reader.read(name, v); }

/* EOF */
