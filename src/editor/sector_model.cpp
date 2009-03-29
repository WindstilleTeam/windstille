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

#include "editor_window.hpp"
#include "windstille_widget.hpp"
#include "util/file_reader.hpp"
#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "object_model_factory.hpp"
#include "sector_model.hpp"

LayerManagerColumns* LayerManagerColumns::instance_ = 0;

SectorModel::SectorModel()
{  
  layer_tree = Gtk::TreeStore::create(LayerManagerColumns::instance());

  Gtk::TreeStore::iterator it = layer_tree->append();

  HardLayerHandle layer(new HardLayer());

  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = Glib::ustring("Scene");
  (*it)[LayerManagerColumns::instance().visible]   = true;
  (*it)[LayerManagerColumns::instance().locked]    = false;
  (*it)[LayerManagerColumns::instance().layer]     = layer;

  layer_tree->signal_row_changed().connect(sigc::mem_fun(*this, &SectorModel::on_row_changed));
  layer_tree->signal_row_deleted().connect(sigc::mem_fun(*this, &SectorModel::on_row_deleted));
  layer_tree->signal_row_has_child_toggled().connect(sigc::mem_fun(*this, &SectorModel::on_row_has_child_toggled));
  layer_tree->signal_row_inserted().connect(sigc::mem_fun(*this, &SectorModel::on_row_inserted));
  layer_tree->signal_rows_reordered().connect(sigc::mem_fun(*this, &SectorModel::on_rows_reordered));
}

void
SectorModel::add_layer(const std::string& name, const Gtk::TreeModel::Path& path)
{
  Gtk::TreeStore::iterator it;

  if (path.empty())
    it = layer_tree->append();
  else
    it = layer_tree->append(layer_tree->get_iter(path)->children());

  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = name;
  (*it)[LayerManagerColumns::instance().visible]   = true; 
  (*it)[LayerManagerColumns::instance().locked]    = false; 
  (*it)[LayerManagerColumns::instance().layer]     = HardLayerHandle(new HardLayer());
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
      Gtk::TreeStore::iterator it = layer_tree->get_iter(path);
      ((HardLayerHandle)(*it)[LayerManagerColumns::instance().layer])->add(object);
    }
}

struct GetLayersFunctor
{
  SectorModel::HardLayers& objects;

  GetLayersFunctor(SectorModel::HardLayers& objects_) 
    : objects(objects_)
  {}

  bool get_layers(const Gtk::TreeModel::iterator& it)
  {
    objects.push_back((*it)[LayerManagerColumns::instance().layer]);
    return false;
  }
};

SectorModel::HardLayers
SectorModel::get_layers() const
{
  HardLayers lst;
  GetLayersFunctor func(lst);
  layer_tree->foreach_iter(sigc::mem_fun(func, &GetLayersFunctor::get_layers));
  return lst;
}

HardLayerHandle
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
          return HardLayerHandle();
        }
    }
  else
    {
      return HardLayerHandle();
    }
}

void
SectorModel::remove(const ObjectModelHandle& object)
{
  const HardLayers& layers = get_layers();
 
  for(HardLayers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
    {
      (*i)->remove(object);
    }
}

void
SectorModel::draw(SceneContext& sc, const Layers& layermask)
{
  const HardLayers& layers = get_layers();
 
  for(HardLayers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
    {
      if ((*i)->is_visible())
        (*i)->draw(sc, layermask);
    }
}

void
SectorModel::update(float delta)
{
  const HardLayers& layers = get_layers();

  for(HardLayers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible())
        (*i)->update(delta);
    }
}

ObjectModelHandle
SectorModel::get_object_at(const Vector2f& pos, const Layers& layermask) const
{
  const HardLayers& layers = get_layers();
  SelectionHandle selection = Selection::create();

  for(HardLayers::const_iterator i = layers.begin(); i != layers.end(); ++i)
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
SectorModel::get_selection(const Rectf& rect, const Layers& layermask) const
{
  const HardLayers& layers = get_layers();
  SelectionHandle selection = Selection::create();

  for(HardLayers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible() && !(*i)->is_locked())
        {
          SelectionHandle new_sel = (*i)->get_selection(rect, layermask);
          selection->add(new_sel->begin(), new_sel->end());
        }
    }

  return selection;
}

HardLayerHandle
SectorModel::get_layer(ObjectModelHandle object)
{
  const HardLayers& layers = get_layers();
  for(HardLayers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->has_object(object))
        return *i;
    }
  return HardLayerHandle();
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
  const HardLayers& layers = get_layers();

  SnapData snap_data;
  for(HardLayers::const_iterator i = layers.begin(); i != layers.end(); ++i)
    {
      if ((*i)->is_visible())
        {
          snap_data.merge((*i)->snap_object(rect, ignore_objects));
        }
    }
  return snap_data;
}

void
SectorModel::load_layer(FileReader reader, const Gtk::TreeModel::Row* parent)
{
  FileReader objects_reader;
  FileReader layers_reader;

  std::string name = "New Layer";
  bool visible = true;;
  bool locked  = false;

  reader.read("name", name);
  reader.read("visible", visible);
  reader.read("locked", locked);
  reader.read("objects", objects_reader);
  reader.read("child-layers", layers_reader);

  //std::cout << "loading layer: " << reader.get_name() << " " << name << " " << visible << " " << locked << std::endl;

  HardLayerHandle layer = HardLayerHandle(new HardLayer());

  const std::vector<FileReader>& objects_sections = objects_reader.get_sections();
  for(std::vector<FileReader>::const_iterator j = objects_sections.begin(); j != objects_sections.end(); ++j)
    {
      layer->add(ObjectModelFactory::create(*j));
    }

  // Append the layer to the tree
  Gtk::TreeStore::iterator it = parent ? layer_tree->append(parent->children()) : it = layer_tree->append();

  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[LayerManagerColumns::instance().name]      = name;
  (*it)[LayerManagerColumns::instance().visible]   = visible; 
  (*it)[LayerManagerColumns::instance().locked]    = locked; 
  (*it)[LayerManagerColumns::instance().layer]     = layer;

  layer->update(*it);
  
  const std::vector<FileReader>& layers_sections = layers_reader.get_sections();
  for(std::vector<FileReader>::const_iterator j = layers_sections.begin(); j != layers_sections.end(); ++j)
    {
      load_layer(*j, &*it);
    }
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
      FileReader reader = FileReader::parse(stream, filename);
      if (reader.get_name() == "windstille-sector")
        {
          const std::vector<FileReader>& sections = reader.get_sections();
          for(std::vector<FileReader>::const_iterator i = sections.begin(); i != sections.end(); ++i)
            {
              if (i->get_name() == "layer")
                {
                  load_layer(*i, 0);
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

  writer.write("version", 1);
  writer.write("name", "");
  writer.write("ambient-color", Color());
  writer.write("init-script", "init.nut");

  write(writer, *(layer_tree->children().begin()));

  writer.end_section();
  writer.write_raw("\n\n;; EOF ;;\n");
}

void
SectorModel::write(FileWriter& writer, const Gtk::TreeRow& row) const
{
  writer.start_section("layer");
  writer.write("name",    (Glib::ustring)(row[LayerManagerColumns::instance().name]));
  writer.write("visible", (bool)row[LayerManagerColumns::instance().visible]);
  writer.write("locked",  (bool)row[LayerManagerColumns::instance().locked]);

  writer.start_section("objects");
  ((HardLayerHandle)row[LayerManagerColumns::instance().layer])->write(writer);
  writer.end_section();

  writer.start_section("child-layers");
  for(Gtk::TreeStore::Children::iterator i = row.children().begin(); i != row.children().end(); ++i)
    {
      write(writer, *i);
    }
  writer.end_section();

  writer.end_section();
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
    ((HardLayerHandle)(*it)[LayerManagerColumns::instance().layer])->update(*it);
    return false;
  }
  
  bool set_locked(const Gtk::TreeModel::iterator& it)
  {
    (*it)[LayerManagerColumns::instance().locked] = v;
    ((HardLayerHandle)(*it)[LayerManagerColumns::instance().layer])->update(*it);
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
SectorModel::on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  //std::cout << "LayerManager:on_row_changed" << std::endl;

  if (iter)
    {
      // Update the Layer object with data from the tree
      HardLayerHandle layer = (*iter)[LayerManagerColumns::instance().layer];
      if (layer)
        {
          layer->update(*iter);
        }
    }
  queue_draw();
}

void
SectorModel::on_row_deleted(const Gtk::TreeModel::Path& path)
{
  //std::cout << "LayerManager:on_row_deleted" << std::endl;
  
  queue_draw();
}

void
SectorModel::on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  //std::cout << "LayerManager:on_row_has_child_toggled" << std::endl;
}

void
SectorModel::on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  //std::cout << "LayerManager:on_row_inserted" << std::endl;
}

void
SectorModel::on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order)
{
  //std::cout << "LayerManager:on_row_reordered" << std::endl;
}

/* EOF */
