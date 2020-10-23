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

#include "editor/sector_model_builder.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <wstdisplay/color.hpp>
#include "editor/layer_manager_columns.hpp"
#include "editor/navigation_graph_model.hpp"
#include "editor/object_model_factory.hpp"
#include "editor/sector_model.hpp"
#include "util/file_reader.hpp"
#include "util/pathname.hpp"

SectorModelBuilder::SectorModelBuilder(const std::string& filename, SectorModel& sector) :
  m_sector(sector),
  m_id_table(),
  m_parent_table()
{
  m_sector.get_layer_tree()->clear();

  ReaderDocument doc = ReaderDocument::from_file(filename);
  if (doc.get_name() == "windstille-sector")
  {
    ReaderMapping const& reader = doc.get_mapping();

    RGBAf ambient_color;
    if (reader.read("ambient-color", ambient_color)) {
      m_sector.set_ambient_color(ambient_color);
    }

    ReaderMapping navigation_map;
    reader.read("navigation", navigation_map);
    m_sector.get_nav_graph().load(navigation_map, m_id_table);

    ReaderCollection layers_collection;
    reader.read("layers", layers_collection);

    // Load layer in reversed order, as ListStore is in reverse
    std::vector<ReaderObject> layers_objs = layers_collection.get_objects();
    std::reverse(layers_objs.begin(), layers_objs.end());
    for (ReaderObject const& item : layers_objs) {
      if (item.get_name() == "layer") {
        load_layer(item.get_mapping());
      } else {
        std::cout << "SectorModel::load: ignoring unknown type '" << item.get_name() << "'" << std::endl;
      }
    }

    // Set the parents properly
    for(std::map<ObjectModelHandle, std::string>::iterator i = m_parent_table.begin(); i != m_parent_table.end(); ++i)
    {
      std::map<std::string, ObjectModelHandle>::iterator j = m_id_table.find(i->second);
      if (j == m_id_table.end())
      {
        std::cout << "Error: Couldn't resolve 'id': " << i->second << std::endl;
      }
      else
      {
        i->first->set_parent(j->second, false);
      }
    }
  }
}

void
SectorModelBuilder::load_layer(ReaderMapping const& reader)
{
  ReaderCollection objects_col;
  std::string name = "New Layer";
  bool visible = true;
  bool locked  = false;

  reader.read("name",    name);
  reader.read("visible", visible);
  reader.read("locked",  locked);
  reader.read("objects", objects_col);

  LayerHandle layer = LayerHandle(new Layer(m_sector));

  for (ReaderObject const& item : objects_col.get_objects()) {
    try {
      if (item.get_name() == "navgraph-edge-ref") {
        ReaderMapping const& map = item.get_mapping();

        std::string id_str;
        if (map.read("edge", id_str)) {
          std::map<std::string, ObjectModelHandle>::iterator it = m_id_table.find(id_str);
          if (it == m_id_table.end()) {
            std::cout << "SectorModel::load_layer: couldn't resource navgraph-edge-ref: " << id_str << std::endl;
          } else {
            layer->add(it->second);
          }
        }
      } else {
        ObjectModelHandle obj = ObjectModelFactory::create(item);

        layer->add(obj);

        ReaderMapping const& map = item.get_mapping();
        std::string id_str;
        if (map.read("id", id_str)) {
          m_id_table[id_str] = obj;
        }

        std::string parent_str;
        if (map.read("parent", parent_str)) {
          if (!parent_str.empty()) {
            m_parent_table[obj] = parent_str;
          }
        }
      }
    } catch(std::exception& err) {
      std::cout << "SectorModel::load_layer: " << err.what() << std::endl;
    }
  }

  // Append the layer to the tree
  Gtk::ListStore::iterator it = m_sector.get_layer_tree()->append();

  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file(Pathname("editor/type.png", Pathname::kDataPath).get_sys_path());
  (*it)[LayerManagerColumns::instance().name]      = name;
  (*it)[LayerManagerColumns::instance().visible]   = visible;
  (*it)[LayerManagerColumns::instance().locked]    = locked;
  (*it)[LayerManagerColumns::instance().layer]     = layer;

  layer->sync(*it);
}

/* EOF */
