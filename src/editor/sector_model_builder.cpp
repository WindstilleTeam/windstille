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

#include "editor/sector_model_builder.hpp"

#include <fstream>
#include <stdexcept>

#include "display/color.hpp"
#include "editor/layer_manager_columns.hpp"
#include "editor/navigation_graph_model.hpp"
#include "editor/object_model_factory.hpp"
#include "editor/sector_model.hpp"
#include "util/file_reader.hpp"

SectorModelBuilder::SectorModelBuilder(const std::string& filename, SectorModel& sector) :
  m_sector(sector),
  m_id_table(),
  m_parent_table()
{
  m_sector.get_layer_tree()->clear();

  std::ifstream stream(filename.c_str());
  if (!stream)
  {
    throw std::runtime_error("File not found " + filename);
  }
  else
  {
    FileReader reader = FileReader::parse(stream, filename);
    if (reader.get_name() == "windstille-sector")
    {
      Color ambient_color;
      if (reader.get("ambient-color", ambient_color))
      {
        m_sector.set_ambient_color(ambient_color);
      }

      FileReader navigation_section;
      reader.get("navigation", navigation_section);
      m_sector.get_nav_graph().load(navigation_section, m_id_table);

      FileReader layers_section;
      reader.get("layers", layers_section);

      const std::vector<FileReader>& sections = layers_section.get_sections();
      // Load layer in reversed order, as ListStore is in reverse
      for(std::vector<FileReader>::const_reverse_iterator i = sections.rbegin(); i != sections.rend(); ++i)
      {
        if (i->get_name() == "layer")
        {
          load_layer(*i);
        }
        else
        {
          std::cout << "SectorModel::load: ignoring unknown type '" << i->get_name() << "'" << std::endl;
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
}

void
SectorModelBuilder::load_layer(const FileReader& reader)
{
  FileReader objects_reader;
  FileReader layers_reader;

  std::string name = "New Layer";
  bool visible = true;;
  bool locked  = false;

  reader.get("name",    name);
  reader.get("visible", visible);
  reader.get("locked",  locked);
  reader.get("objects", objects_reader);

  LayerHandle layer = LayerHandle(new Layer(m_sector));

  const std::vector<FileReader>& objects_sections = objects_reader.get_sections();
  for(std::vector<FileReader>::const_iterator j = objects_sections.begin(); j != objects_sections.end(); ++j)
  {
    try
    {
      if (j->get_name() == "navgraph-edge-ref")
      {
        std::string id_str;
        if (j->get("edge", id_str))
        {
          std::map<std::string, ObjectModelHandle>::iterator it = m_id_table.find(id_str);
          if (it == m_id_table.end())
          {
            std::cout << "SectorModel::load_layer: couldn't resource navgraph-edge-ref: " << id_str << std::endl;
          }
          else
          {
            layer->add(it->second);
          }
        }
      }
      else
      {
        ObjectModelHandle obj = ObjectModelFactory::create(*j);

        layer->add(obj);

        std::string id_str;
        if (j->read("id", id_str))
        {
          m_id_table[id_str] = obj;
        }

        std::string parent_str;
        if (j->read("parent", parent_str))
        {
          if (!parent_str.empty())
            m_parent_table[obj] = parent_str;
        }
      }
    }
    catch(std::exception& err)
    {
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
