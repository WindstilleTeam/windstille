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

#include <fstream>
#include <limits>
#include <assert.h>
#include <iostream>
#include <gdkmm/pixbuf.h>

#include "navigation/navigation_graph.hpp"
#include "navigation/edge.hpp"
#include "navigation/node.hpp"
#include "editor/editor_window.hpp"
#include "editor/windstille_widget.hpp"
#include "util/file_reader.hpp"
#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "editor/object_model_factory.hpp"
#include "editor/sector_model.hpp"

LayerManagerColumns* LayerManagerColumns::instance_ = 0;

SectorModel::SectorModel()
  : nav_graph(new NavigationGraph()),
    layer_tree(),
    ambient_color()
{
  layer_tree = Gtk::ListStore::create(LayerManagerColumns::instance());

  Gtk::ListStore::iterator it = layer_tree->append();

  LayerHandle layer(new Layer());
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = Glib::ustring("Scene");
  (*it)[LayerManagerColumns::instance().visible]   = true;
  (*it)[LayerManagerColumns::instance().locked]    = false;
  (*it)[LayerManagerColumns::instance().layer]     = layer;
  layer->sync(*it);

  layer_tree->signal_row_changed().connect(sigc::mem_fun(*this, &SectorModel::on_row_changed));
  layer_tree->signal_row_deleted().connect(sigc::mem_fun(*this, &SectorModel::on_row_deleted));
  layer_tree->signal_row_has_child_toggled().connect(sigc::mem_fun(*this, &SectorModel::on_row_has_child_toggled));
  layer_tree->signal_row_inserted().connect(sigc::mem_fun(*this, &SectorModel::on_row_inserted));
  layer_tree->signal_rows_reordered().connect(sigc::mem_fun(*this, &SectorModel::on_rows_reordered));
}

SectorModel::~SectorModel()
{
}

void
SectorModel::add_layer(LayerHandle layer, const Gtk::TreeModel::Path& path)
{
 Gtk::ListStore::iterator it;

  if (path.empty())
    it = layer_tree->append();
  else
    it = layer_tree->insert(layer_tree->get_iter(path));
  
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = layer->get_name();
  (*it)[LayerManagerColumns::instance().visible]   = layer->is_visible();
  (*it)[LayerManagerColumns::instance().locked]    = layer->is_locked();
  (*it)[LayerManagerColumns::instance().layer]     = layer;
}

void
SectorModel::add_layer(const std::string& name, const Gtk::TreeModel::Path& path)
{
  Gtk::ListStore::iterator it;

  if (path.empty())
    it = layer_tree->append();
  else
    it = layer_tree->insert(layer_tree->get_iter(path));

  LayerHandle layer(new Layer());
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = name;
  (*it)[LayerManagerColumns::instance().visible]   = true; 
  (*it)[LayerManagerColumns::instance().locked]    = false; 
  (*it)[LayerManagerColumns::instance().layer]     = layer;
  layer->sync(*it);
}

void
SectorModel::delete_layer(const Gtk::TreeModel::Path& path)
{
  if (path.empty())
    {
      EditorWindow::current()->print("SectorModel::delete_layer(): invalid empty path");
    }
  else
    {
      layer_tree->erase(layer_tree->get_iter(path));
    }
}

void
SectorModel::add(const ObjectModelHandle& object, const Gtk::TreeModel::Path& path)
{
  if (path.empty())
    {
      EditorWindow::current()->print("SectorModel::add(): invalid empty path");
    }
  else
    { 
      Gtk::ListStore::iterator it = layer_tree->get_iter(path);
      ((LayerHandle)(*it)[LayerManagerColumns::instance().layer])->add(object);
    }
}

struct GetLayersFunctor
{
  SectorModel::Layers& objects;

  GetLayersFunctor(SectorModel::Layers& objects_) 
    : objects(objects_)
  {}

  bool get_layers(const Gtk::TreeModel::iterator& it)
  {
    objects.push_back((*it)[LayerManagerColumns::instance().layer]);
    return false;
  }
};

SectorModel::Layers
SectorModel::get_layers() const
{
  Layers lst;
  GetLayersFunctor func(lst);
  layer_tree->foreach_iter(sigc::mem_fun(func, &GetLayersFunctor::get_layers));
  return lst;
}

LayerHandle
SectorModel::get_layer(const Gtk::TreeModel::Path& path) const
{
  if (!path.empty())
    {
      Gtk::TreeModel::iterator it = layer_tree->get_iter(path);
      if (it)
        {
          return (*it)[LayerManagerColumns::instance().layer];
        }
      else
        {
          return LayerHandle();
        }
    }
  else
    {
      return LayerHandle();
    }
}

void
SectorModel::remove(const ObjectModelHandle& object)
{
  const Layers& layers = get_layers();
 
  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
    {
      (*i)->remove(object);
    }
}

LayerHandle
SectorModel::get_layer(const ObjectModelHandle& object) const
{
  const Layers& layers = get_layers();
 
  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
    {
      if ((*i)->has_object(object))
        {
          return *i;
        }
    }  
  
  return LayerHandle();
}

void
SectorModel::draw(SceneContext& sc, const SelectMask& layermask, bool draw_navgraph)
{
  // Draw Layers
  const Layers& layers = get_layers();
 
  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
    {
      if ((*i)->is_visible())
        (*i)->draw(sc, layermask);
    }

  // Draw Navgraph
  if (draw_navgraph)
    {
      for(NavigationGraph::Edges::iterator i = nav_graph->get_edges().begin(); i != nav_graph->get_edges().end(); ++i)
        {
          sc.control().draw_line(Line((*i)->get_node1()->get_pos(),
                                      (*i)->get_node2()->get_pos()),
                                 Color(1.0f, 0.0f, 0.0f));
        }

      for(NavigationGraph::Nodes::iterator i = nav_graph->get_nodes().begin(); i != nav_graph->get_nodes().end(); ++i)
        {
          sc.control().fill_rect(Rectf((*i)->get_pos() - Vector2f(4,4), Sizef(9, 9)),
                                 Color(1.0f, 1.0f, 0.0f));
        }
    }
}

void
SectorModel::update(float delta)
{
  const Layers& layers = get_layers();

  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible())
        (*i)->update(delta);
    }
}

ObjectModelHandle
SectorModel::get_object_at(const Vector2f& pos, const SelectMask& layermask) const
{
  const Layers& layers = get_layers();
  SelectionHandle selection = Selection::create();

  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible() && !(*i)->is_locked())
        {
          ObjectModelHandle object = (*i)->get_object_at(pos, layermask);
          
          if (object.get())
            return object;
        }
    }

  return ObjectModelHandle();
}

SelectionHandle
SectorModel::get_selection(const Rectf& rect, const SelectMask& layermask) const
{
  const Layers& layers = get_layers();
  SelectionHandle selection = Selection::create();

  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible() && !(*i)->is_locked())
        {
          SelectionHandle new_sel = (*i)->get_selection(rect, layermask);
          selection->add(new_sel->begin(), new_sel->end());
        }
    }

  return selection;
}

LayerHandle
SectorModel::get_layer(ObjectModelHandle object)
{
  const Layers& layers = get_layers();
  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->has_object(object))
        return *i;
    }
  return LayerHandle();
}

void
SectorModel::raise(ObjectModelHandle object)
{
  get_layer(object)->raise(object);
}

void
SectorModel::lower(ObjectModelHandle object)
{
  get_layer(object)->lower(object);
}

void
SectorModel::raise_to_top(ObjectModelHandle object)
{
  get_layer(object)->raise_to_top(object);
}

void
SectorModel::lower_to_bottom(ObjectModelHandle object)
{
  get_layer(object)->lower_to_bottom(object);
}

SnapData
SectorModel::snap_object(const Rectf& rect, const std::set<ObjectModelHandle>& ignore_objects) const
{
  const Layers& layers = get_layers();

  SnapData snap_data;
  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible())
        {
          snap_data.merge((*i)->snap_object(rect, ignore_objects));
        }
    }
  return snap_data;
}

void
SectorModel::load_layer(const FileReader& reader, 
                        std::map<std::string, ObjectModelHandle>& id_table,
                        std::map<ObjectModelHandle, std::string>& parent_table)
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

  //std::cout << "loading layer: " << reader.get_name() << " " << name << " " << visible << " " << locked << std::endl;

  LayerHandle layer = LayerHandle(new Layer());

  const std::vector<FileReader>& objects_sections = objects_reader.get_sections();
  for(std::vector<FileReader>::const_iterator j = objects_sections.begin(); j != objects_sections.end(); ++j)
    {
      ObjectModelHandle obj = ObjectModelFactory::create(*j);

      layer->add(obj);

      std::string id_str;
      if (j->read("id", id_str))
        {
          id_table[id_str] = obj;
        }

      std::string parent_str;
      if (j->read("parent", parent_str))
        {
          if (!parent_str.empty())
            parent_table[obj] = parent_str;
        }
    }

  // Append the layer to the tree
  Gtk::ListStore::iterator it = layer_tree->append();

  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = name;
  (*it)[LayerManagerColumns::instance().visible]   = visible; 
  (*it)[LayerManagerColumns::instance().locked]    = locked; 
  (*it)[LayerManagerColumns::instance().layer]     = layer;

  layer->sync(*it);
}

void
SectorModel::load(const std::string& filename)
{
  layer_tree->clear();

  std::ifstream stream(filename.c_str());
  if (!stream)
    {
      throw std::runtime_error("File not found " + filename);
    }
  else
    {
      std::map<std::string, ObjectModelHandle> id_table;
      std::map<ObjectModelHandle, std::string> parent_table;

      FileReader reader = FileReader::parse(stream, filename);
      if (reader.get_name() == "windstille-sector")
        {
          ambient_color = Color(0,0,0,1);
          reader.get("ambient-color", ambient_color);

          FileReader navigation_section;
          reader.get("navigation", navigation_section);
          nav_graph->load(navigation_section);

          FileReader layers_section;
          reader.get("layers", layers_section);

          const std::vector<FileReader>& sections = layers_section.get_sections();
          for(std::vector<FileReader>::const_iterator i = sections.begin(); i != sections.end(); ++i)
            {
              load_layer(*i, id_table, parent_table);
            }
          
          // Set the parents properly
          for(std::map<ObjectModelHandle, std::string>::iterator i = parent_table.begin(); i != parent_table.end(); ++i)
            {
              std::map<std::string, ObjectModelHandle>::iterator j = id_table.find(i->second);
              if (j == id_table.end())
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
SectorModel::write(FileWriter& writer) const
{
  writer.write_raw(";; -*- scheme -*-\n");
  writer.start_section("windstille-sector");

  writer.write("version", 3);
  writer.write("name", "");
  writer.write("ambient-color", ambient_color);
  writer.write("init-script", "init.nut");

  writer.start_section("navigation");
  nav_graph->write(writer);
  writer.end_section();

  writer.start_section("layers");
  for(Gtk::ListStore::Children::iterator i = layer_tree->children().begin(); i != layer_tree->children().end(); ++i)
    {
      const Gtk::TreeRow& row = *i;

      writer.start_section("layer");
      writer.write("name",    (Glib::ustring)(row[LayerManagerColumns::instance().name]));
      writer.write("visible", (bool)row[LayerManagerColumns::instance().visible]);
      writer.write("locked",  (bool)row[LayerManagerColumns::instance().locked]);

      writer.start_section("objects");
      ((LayerHandle)row[LayerManagerColumns::instance().layer])->write(writer);
      writer.end_section();

      writer.end_section();
    }
  writer.end_section();

  writer.end_section();
  writer.write_raw("\n\n;; EOF ;;\n");
}

struct PropSetFunctor
{
  bool v;

  PropSetFunctor(bool v_)
    : v(v_)
  {}

  bool set_visible(const Gtk::TreeModel::iterator& it)
  {
    (*it)[LayerManagerColumns::instance().visible] = v;
    ((LayerHandle)(*it)[LayerManagerColumns::instance().layer])->sync(*it);
    return false;
  }
  
  bool set_locked(const Gtk::TreeModel::iterator& it)
  {
    (*it)[LayerManagerColumns::instance().locked] = v;
    ((LayerHandle)(*it)[LayerManagerColumns::instance().layer])->sync(*it);
    return false;
  }
};

void
SectorModel::set_all_visible(bool v)
{
  PropSetFunctor func(v);
  layer_tree->foreach_iter(sigc::mem_fun(func, &PropSetFunctor::set_visible));
}

void
SectorModel::set_all_locked(bool v)
{
  PropSetFunctor func(v);
  layer_tree->foreach_iter(sigc::mem_fun(func, &PropSetFunctor::set_locked));
}

void
SectorModel::queue_draw()
{
  if (WindstilleWidget* wst = EditorWindow::current()->get_windstille_widget())
    {
      wst->queue_draw();
    }
}

void
SectorModel::on_row_changed(const Gtk::TreeModel::Path& /*path*/, const Gtk::TreeModel::iterator& iter)
{
  //std::cout << "LayerManager:on_row_changed" << std::endl;

  if (iter)
    {
      // Update the Layer object with data from the tree
      LayerHandle layer = (*iter)[LayerManagerColumns::instance().layer];
      if (layer)
        {
          layer->sync(*iter);
        }
    }

  queue_draw();
}

void
SectorModel::on_row_deleted(const Gtk::TreeModel::Path& /*path*/)
{
  //std::cout << "LayerManager:on_row_deleted" << std::endl;
  queue_draw();
}

void
SectorModel::on_row_has_child_toggled(const Gtk::TreeModel::Path& /*path*/, const Gtk::TreeModel::iterator& /*iter*/)
{
  //std::cout << "LayerManager:on_row_has_child_toggled" << std::endl;
}

void
SectorModel::on_row_inserted(const Gtk::TreeModel::Path& /*path*/, const Gtk::TreeModel::iterator& /*iter*/)
{
  //std::cout << "LayerManager:on_row_inserted" << std::endl;
}

void
SectorModel::on_rows_reordered(const Gtk::TreeModel::Path& /*path*/, const Gtk::TreeModel::iterator& /*iter*/, int* /*new_order*/)
{
  //std::cout << "LayerManager:on_row_reordered" << std::endl;
}

/* EOF */
