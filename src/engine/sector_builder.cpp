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

#include "engine/sector_builder.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "app/globals.hpp"
#include "display/color.hpp"
#include "engine/game_object.hpp"
#include "engine/object_factory.hpp"
#include "engine/sector.hpp"
#include "navigation/navigation_graph.hpp"
#include "tile/tile_map.hpp"
#include "util/file_reader.hpp"

SectorBuilder::SectorBuilder(const Pathname& filename, Sector& sector) :
  m_filename(filename),
  m_sector(sector),
  id_table(),
  parent_table()
{
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
}

void
SectorBuilder::parse_layer(const FileReader& reader)
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
      parse_object(*i);
    }
  }
}

void
SectorBuilder::parse_objects(const FileReader& reader)
{
  FileReader objects_reader;

  // FIXME: try both for backward compatibility
  if (!reader.get("objects", objects_reader) &&
      !reader.get("layers", objects_reader))
  {
    throw std::runtime_error("No objects specified");
  }
  else
  {
    std::vector<FileReader> objects_readers = objects_reader.get_sections();
    for(std::vector<FileReader>::iterator i = objects_readers.begin(); i != objects_readers.end(); ++i)
    {
      parse_object(*i);
    }

    // Set the parents properly
    for(std::map<GameObjectHandle, std::string>::iterator i = parent_table.begin(); i != parent_table.end(); ++i)
    {
      std::map<std::string, GameObjectHandle>::iterator j = id_table.find(i->second);
      if (j == id_table.end())
      {
        std::cout << "Error: Couldn't resolve 'id': " << i->second << std::endl;
      }
      else
      {
        i->first->set_gameobj_parent(j->second);
      }
    }
  }
}

void
SectorBuilder::parse_object(const FileReader& reader)
{
  GameObjectHandle obj;

  if(reader.get_name() == "tilemap")
  {
    std::unique_ptr<TileMap> tilemap(new TileMap(reader));

    if (tilemap->get_name() == "interactive")
      m_sector.interactive_tilemap = tilemap.get();
    else if (tilemap->get_name() == "interactivebackground")
      m_sector.interactivebackground_tilemap = tilemap.get();

    obj.reset(tilemap.release());
  }
  else if (reader.get_name() == "navgraph-edge-ref")
  {
    // FIXME: Implement me
  }
  else if(reader.get_name() == "background")
  {
    // TODO
  }
  else if(reader.get_name() == "layer")
  {
    parse_layer(reader);
  }
  else
  {
    obj = ObjectFactory::create(reader);
    if (!obj)
    {
      std::cout << "Skipping unknown Object: " << reader.get_name() << "\n";
    }
  }

  if (obj)
  {
    std::string id_str;
    if (reader.read("id", id_str))
    {
      id_table[id_str] = obj;
    }

    std::string parent_str;
    if (reader.read("parent", parent_str))
    {
      if (!parent_str.empty())
        parent_table[obj] = parent_str;
    }

    m_sector.add(obj);
  }
}

void
SectorBuilder::parse_navgraph(const FileReader& reader)
{
  std::map<std::string, NodeHandle> id_to_node;

  FileReader navgraph_reader;
  if (!reader.get("navigation", navgraph_reader))
  {
    // throw std::runtime_error("SectorBuilder: 'navigation' section missing");
    std::cout << "SectorBuilder: 'navigation' section missing" << std::endl;
  }
  else
  {
    FileReader nodes_reader;
    if (navgraph_reader.get("nodes", nodes_reader))
    {
      const std::vector<FileReader>& nodes_readers = nodes_reader.get_sections();
      for(std::vector<FileReader>::const_iterator i = nodes_readers.begin(); i != nodes_readers.end(); ++i)
      {
        if (i->get_name() != "navgraph-node")
        {
          std::cout << "SectorBuilder::parse_navgraph(): Unknown nodes tag: " << i->get_name() << std::endl;
        }
        else
        {
          Vector2f pos;
          if (i->get("pos", pos))
          {
            NodeHandle node = m_sector.get_navigation_graph().add_node(pos);
            std::string id;
            if (i->get("id", id))
            {
              id_to_node[id] = node;
            }
          }
        }
      }
    }

    FileReader edges_reader;
    if (navgraph_reader.get("edges", edges_reader))
    {
      const std::vector<FileReader>& edges_readers = edges_reader.get_sections();
      for(std::vector<FileReader>::const_iterator i = edges_readers.begin(); i != edges_readers.end(); ++i)
      {
        if (i->get_name() != "navgraph-edge")
        {
          std::cout << "SectorBuilder::parse_navgraph(): Unknown edges tag: " << i->get_name() << std::endl;
        }
        else
        {
          std::string lhs_node;
          std::string rhs_node;
          if (i->get("lhs-node", lhs_node) &&
              i->get("rhs-node", rhs_node))
          {
            std::map<std::string, NodeHandle>::iterator lhs = id_to_node.find(lhs_node);
            std::map<std::string, NodeHandle>::iterator rhs = id_to_node.find(rhs_node);
            if (lhs != id_to_node.end() &&
                rhs != id_to_node.end())
            {
              m_sector.get_navigation_graph().add_edge(lhs->second, rhs->second);
            }
          }
        }
      }
    }
  }
}

/* EOF */
