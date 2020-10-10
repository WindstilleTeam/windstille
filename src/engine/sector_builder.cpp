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
}

void
SectorBuilder::build()
{
  ReaderDocument doc = ReaderDocument::from_file(m_filename.get_sys_path(), true);
  if (doc.get_name() != "windstille-sector")
  {
    std::ostringstream msg;
    msg << "'" << m_filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  else
  {
    parse_body(doc.get_mapping());
  }
}

void
SectorBuilder::parse_body(ReaderMapping const& reader)
{
  { // check version
    int version = 1;
    if (!reader.read("version", version))
      std::cerr << "Warning no version specified in levelformat.\n";

    if (version > 3)
      std::cerr << "Warning: format version is newer than game.\n";
  }

  //reader.read("name",          name);
  //reader.read("music",         music);
  //reader.read("init-script",   init_script);

  Color ambient_light;
  if (reader.read("ambient-color", ambient_light))
  {
    m_sector.set_ambient_light(ambient_light);
  }

  parse_objects(reader);
  parse_navgraph(reader);
}

void
SectorBuilder::parse_layer(ReaderMapping const& reader)
{
  ReaderCollection objects_collection;
  if (!reader.read("objects", objects_collection)) {
    throw std::runtime_error("No objects specified");
  }

  for(ReaderObject const& item : objects_collection.get_objects()) {
    parse_object(item);
  }
}

void
SectorBuilder::parse_objects(ReaderMapping const& reader)
{
  ReaderCollection objects_collection;

  // FIXME: try both for backward compatibility
  if (!reader.read("objects", objects_collection) &&
      !reader.read("layers", objects_collection)) {
    throw std::runtime_error("No objects specified");
  }

  for(ReaderObject const& obj : objects_collection.get_objects())
  {
    parse_object(obj);
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

void
SectorBuilder::parse_object(ReaderObject const& reader_obj)
{
  GameObjectHandle obj;

  if (reader_obj.get_name() == "tilemap")
  {
    std::unique_ptr<TileMap> tilemap(new TileMap(reader_obj.get_mapping()));

    if (tilemap->get_name() == "interactive")
      m_sector.interactive_tilemap = tilemap.get();
    else if (tilemap->get_name() == "interactivebackground")
      m_sector.interactivebackground_tilemap = tilemap.get();

    obj.reset(tilemap.release());
  }
  else if (reader_obj.get_name() == "navgraph-edge-ref")
  {
    // FIXME: Implement me
  }
  else if(reader_obj.get_name() == "background")
  {
    // TODO
  }
  else if(reader_obj.get_name() == "layer")
  {
    parse_layer(reader_obj.get_mapping());
  }
  else
  {
    obj = ObjectFactory::create(reader_obj);
    if (!obj)
    {
      std::cout << "Skipping unknown Object: " << reader_obj.get_name() << "\n";
    }
  }

  if (obj)
  {
    std::string id_str;
    ReaderMapping const& reader_map = reader_obj.get_mapping();
    if (reader_map.read("id", id_str))
    {
      id_table[id_str] = obj;
    }

    std::string parent_str;
    if (reader_map.read("parent", parent_str))
    {
      if (!parent_str.empty())
        parent_table[obj] = parent_str;
    }

    m_sector.add(obj);
  }
}

void
SectorBuilder::parse_navgraph(ReaderMapping const& reader)
{
  std::map<std::string, NodeHandle> id_to_node;

  ReaderMapping navgraph_reader;
  if (!reader.read("navigation", navgraph_reader)) {
    // throw std::runtime_error("SectorBuilder: 'navigation' section missing");
    std::cout << "SectorBuilder: 'navigation' section missing" << std::endl;
    return;
  }

  ReaderCollection nodes_collection;
  if (navgraph_reader.read("nodes", nodes_collection))
  {
    for (ReaderObject const& item : nodes_collection.get_objects()) {
      if (item.get_name() != "navgraph-node") {
        std::cout << "SectorBuilder::parse_navgraph(): Unknown nodes tag: " << item.get_name() << std::endl;
      } else {
        ReaderMapping const& map = item.get_mapping();
        Vector2f pos;
        if (map.read("pos", pos)) {
          NodeHandle node = m_sector.get_navigation_graph().add_node(pos);
          std::string id;
          if (map.read("id", id))
          {
            id_to_node[id] = node;
          }
        }
      }
    }
  }

  ReaderCollection edges_collection;
  if (navgraph_reader.read("edges", edges_collection))
  {
    for (ReaderObject const& item : edges_collection.get_objects())
    {
      if (item.get_name() != "navgraph-edge") {
        std::cout << "SectorBuilder::parse_navgraph(): Unknown edges tag: " << item.get_name() << std::endl;
      } else {
        ReaderMapping const& map = item.get_mapping();
        std::string lhs_node;
        std::string rhs_node;
        if (map.read("lhs-node", lhs_node) &&
            map.read("rhs-node", rhs_node))
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

/* EOF */
