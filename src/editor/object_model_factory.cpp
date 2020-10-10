/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <stdexcept>

#include "util/file_reader.hpp"
#include "editor/decal_object_model.hpp"
#include "editor/object_model_factory.hpp"

ObjectModelHandle
ObjectModelFactory::create(ReaderObject const& reader_obj)
{
  if (reader_obj.get_name() == "decal") {
    return ObjectModelHandle(new DecalObjectModel(reader_obj.get_mapping()));
  } else {
    throw std::runtime_error("Unknown object type '" + reader_obj.get_name() + "'");
  }
}

/* EOF */
