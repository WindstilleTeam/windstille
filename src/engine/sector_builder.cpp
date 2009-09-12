/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "engine/sector_builder.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "app/globals.hpp"
#include "display/color.hpp"
#include "engine/game_object.hpp"
#include "engine/sector.hpp"
#include "util/file_reader.hpp"

SectorBuilder::SectorBuilder(const Pathname& filename, Sector& sector)
  : m_filename(filename),
    m_sector(sector),
    id_table(),
    parent_table()
{
  if (debug) std::cout << "Sector:parse_file '" << m_filename << "'" << std::endl;
  
  FileReader reader = FileReader::parse(m_filename);
  if(reader.get_name() != "windstille-sector") 
  {
    std::ostringstream msg;
    msg << "'" << m_filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  else
  {
    parse_body(reader);
  }
}

void
SectorBuilder::parse_body(const FileReader& reader)
{
  { // check version
    int version = 1;
    if (!reader.get("version", version))
      std::cerr << "Warning no version specified in levelformat.\n";

    if (version > 3)
      std::cerr << "Warning: format version is newer than game.\n";
  }

  //reader.get("name",          name);
  //reader.get("music",         music);
  //reader.get("init-script",   init_script);

  Color ambient_light;
  if (reader.get("ambient-color", ambient_light))
  {
    m_sector.set_ambient_light(ambient_light);
  }
    
  parse_objects(reader);
  parse_navgraph(reader);

  if (debug) std::cout << "Finished parsing" << std::endl;
}

void
SectorBuilder::parse_objects(const FileReader& reader)
{
  FileReader objects_reader;
  if (!reader.get("objects", objects_reader))
  {
    throw std::runtime_error("No objects specified");
  }
  else
  {
    std::vector<FileReader> objects_readers = objects_reader.get_sections();
    for(std::vector<FileReader>::iterator i = objects_readers.begin(); i != objects_readers.end(); ++i)
    {
      m_sector.add_object(*i);
    }

    // Set the parents properly
    for(std::map<GameObject*, std::string>::iterator i = parent_table.begin(); i != parent_table.end(); ++i)
    {
      std::map<std::string, GameObject*>::iterator j = id_table.find(i->second);
      if (j == id_table.end())
      {
        std::cout << "Error: Couldn't resolve 'id': " << i->second << std::endl;
      }
      else
      {
        i->first->set_parent(j->second);
      }
    }
  }
}

void
SectorBuilder::parse_navgraph(const FileReader& reader)
{
  FileReader navgraph_reader;
  if (!reader.get("navigation", navgraph_reader))
  {
    throw std::runtime_error("SectorBuilder: 'navigation' section missing");
  }
  else
  {
    FileReader nodes_reader;
    if (navgraph_reader.get("nodes", nodes_reader))
    {
    }

    FileReader edges_reader;
    if (navgraph_reader.get("edges", edges_reader))
    {
    }
  }
}

/* EOF */
