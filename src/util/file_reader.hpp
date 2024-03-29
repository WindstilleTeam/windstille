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

#ifndef HEADER_WINDSTILLE_UTIL_FILE_READER_HPP
#define HEADER_WINDSTILLE_UTIL_FILE_READER_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <prio/reader_collection.hpp>
#include <prio/reader_document.hpp>
#include <prio/reader_mapping.hpp>
#include <prio/reader_object.hpp>
#include <surf/fwd.hpp>

namespace windstille {

using ReaderCollection = prio::ReaderCollection;
using ReaderDocument = prio::ReaderDocument;
using ReaderMapping = prio::ReaderMapping;
using ReaderObject = prio::ReaderObject;

} // windstille

namespace prio {

template<> bool read_custom(ReaderMapping const& map, std::string_view key, std::filesystem::path& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, glm::mat4& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, glm::quat& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, surf::Color& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, glm::vec2& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, glm::vec3& value);
template<> bool read_custom(ReaderMapping const& map, std::string_view key, std::vector<unsigned short>& values);

} // namespace prio

#endif

/* EOF */
