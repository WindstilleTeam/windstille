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

#ifndef HEADER_WINDSTILLE_ENGINE_SECTOR_BUILDER_HPP
#define HEADER_WINDSTILLE_ENGINE_SECTOR_BUILDER_HPP

#include <map>
#include <string>

#include "engine/game_object_handle.hpp"
#include "util/file_reader.hpp"

namespace windstille {

class GameObject;
class Pathname;
class Sector;

class SectorBuilder
{
private:
  Pathname const& m_filename;
  Sector&  m_sector;
  std::map<std::string, GameObjectHandle> id_table;
  std::map<GameObjectHandle, std::string> parent_table;

public:
  SectorBuilder(Pathname const& filename, Sector& sector);

  void build();

private:
  void parse_body(ReaderMapping const& reader);
  void parse_layer(ReaderMapping const& reader);
  void parse_objects(ReaderMapping const& reader);
  void parse_object(ReaderObject const& reader_obj);
  void parse_navgraph(ReaderMapping const& reader);

private:
  SectorBuilder(SectorBuilder const&);
  SectorBuilder& operator=(SectorBuilder const&);
};

} // namespace windstille

#endif

/* EOF */
