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

#ifndef HEADER_WINDSTILLE_EDITOR_SECTOR_MODEL_BUILDER_HPP
#define HEADER_WINDSTILLE_EDITOR_SECTOR_MODEL_BUILDER_HPP

#include <string>
#include <map>

#include "object_model.hpp"

class SectorModel;
class FileReader;

class SectorModelBuilder
{
private:
  SectorModel& m_sector;
  std::map<std::string, ObjectModelHandle> m_id_table;
  std::map<ObjectModelHandle, std::string> m_parent_table;

public:
  SectorModelBuilder(const std::string& filename, SectorModel& sector);

private:
  void load(const std::string& filename);
  void load_layer(const FileReader& filename);

private:
  SectorModelBuilder(const SectorModelBuilder&);
  SectorModelBuilder& operator=(const SectorModelBuilder&);
};

#endif

/* EOF */
