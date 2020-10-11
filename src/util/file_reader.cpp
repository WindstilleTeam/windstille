/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/file_reader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "math/vector2f.hpp"
#include "math/quaternion.hpp"
#include "display/color.hpp"
#include "util/file_reader.hpp"

namespace prio {

template<>
bool read_custom(ReaderMapping const& map, std::string_view key, Vector2f&  value)
{
  std::vector<float> floats;
  if (map.read(key, floats) && floats.size() == 2) {
    value.x = floats[0];
    value.y = floats[1];
    return true;
  } else {
    return false;
  }
}

template<>
bool read_custom(ReaderMapping const& map, std::string_view key, Vector3& value)
{
  std::vector<float> floats;
  if (map.read(key, floats) && floats.size() == 3) {
    value.x = floats[0];
    value.y = floats[1];
    value.z = floats[2];
    return true;
  } else {
    return false;
  }
}

template<>
bool read_custom(ReaderMapping const& map, std::string_view key, Quaternion& value)
{
  std::vector<float> floats;
  if (map.read(key, floats) && floats.size() == 4) {
    value.w = floats[0];
    value.x = floats[1];
    value.y = floats[2];
    value.z = floats[3];
    return true;
  } else {
    return false;
  }
}

template<>
bool read_custom(ReaderMapping const& map, std::string_view key, Matrix& m)
{
  std::vector<float> floats;
  if (map.read(key, floats) && floats.size() == 16) {
    float* mp = glm::value_ptr(m);
    mp[ 0] = floats[0];
    mp[ 4] = floats[1];
    mp[ 8] = floats[2];
    mp[12] = floats[3];
    mp[ 1] = floats[4];
    mp[ 5] = floats[5];
    mp[ 9] = floats[6];
    mp[13] = floats[7];
    mp[ 2] = floats[8];
    mp[ 6] = floats[9];
    mp[10] = floats[10];
    mp[14] = floats[11];
    mp[ 3] = floats[12];
    mp[ 7] = floats[13];
    mp[11] = floats[14];
    mp[15] = floats[15];
    return true;
    // FIXME: Could add code to handel 3x3 matrixes
  } else {
    return false;
  }
}

template<>
bool read_custom(ReaderMapping const& map, std::string_view key, Color& value)
{
  std::vector<float> floats;
  if (map.read(key, floats)) {
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
      value.g = floats[1];
      value.b = floats[2];
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

} // namespace prio

/* EOF */
