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

ObjectTreeColumns* ObjectTreeColumns::instance_ = 0;

SectorModel::SectorModel()
{  
  root_layer = HardLayerHandle(new HardLayer());

  layer_tree = Gtk::TreeStore::create(ObjectTreeColumns::instance());

  root_it = layer_tree->append();
  (*root_it)[ObjectTreeColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*root_it)[ObjectTreeColumns::instance().name]      = Glib::ustring("Scene");
  (*root_it)[ObjectTreeColumns::instance().visible]   = true;
  (*root_it)[ObjectTreeColumns::instance().layer]     = root_layer;

  layer_tree->signal_row_changed().connect(sigc::mem_fun(*this, &SectorModel::on_row_changed));
  layer_tree->signal_row_deleted().connect(sigc::mem_fun(*this, &SectorModel::on_row_deleted));
  layer_tree->signal_row_has_child_toggled().connect(sigc::mem_fun(*this, &SectorModel::on_row_has_child_toggled));
  layer_tree->signal_row_inserted().connect(sigc::mem_fun(*this, &SectorModel::on_row_inserted));
  layer_tree->signal_rows_reordered().connect(sigc::mem_fun(*this, &SectorModel::on_rows_reordered));
}

void
SectorModel::add_layer(const std::string& name, const Gtk::TreeModel::Path& path)
{
  Gtk::TreeStore::iterator it = layer_tree->append(layer_tree->get_iter(path)->children());

  (*it)[ObjectTreeColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file("data/editor/type.png");
  (*it)[ObjectTreeColumns::instance().name]      = name;
  (*it)[ObjectTreeColumns::instance().visible]   = true; 
  (*it)[ObjectTreeColumns::instance().layer]     = HardLayerHandle(new HardLayer());
}

void
SectorModel::delete_layer(const Gtk::TreeModel::Path& path)
{
  layer_tree->erase(layer_tree->get_iter(path));
}

void
SectorModel::add(const ObjectModelHandle& object, const Gtk::TreeModel::Path& path)
{
  Gtk::TreeStore::iterator it = layer_tree->get_iter(path);
  ((HardLayerHandle)(*it)[ObjectTreeColumns::instance().layer])->add(object);
}

struct GetLayersFunctor
{
  SectorModel::HardLayers& objects;

  GetLayersFunctor(SectorModel::HardLayers& objects_) 
    : objects(objects_)
  {}

  bool get_layers(const Gtk::TreeModel::iterator& it)
  {
    objects.push_back((*it)[ObjectTreeColumns::instance().layer]);
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

void
SectorModel::raise(ObjectModelHandle object)
{
  root_layer->raise(object);
}

void
SectorModel::lower(ObjectModelHandle object)
{
  root_layer->lower(object);
}

void
SectorModel::raise_to_top(ObjectModelHandle object)
{
  root_layer->raise_to_top(object);
}

void
SectorModel::lower_to_bottom(ObjectModelHandle object)
{
  root_layer->lower_to_bottom(object);
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
SectorModel::load(const std::string& filename)
{
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
          std::vector<FileReader> sections = reader.get_sections();
          for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
            {
              if (i->get_name() == "layer")
                {
                  FileReader objects_reader;
                  std::string name;
                  bool visible;

                  i->read("name", name);
                  i->read("visible", visible);
                  i->read("objects", objects_reader);
                  
                  std::vector<FileReader> objects_sections = objects_reader.get_sections();
                  for(std::vector<FileReader>::iterator j = objects_sections.begin(); j != objects_sections.end(); ++j)
                    {
                      //FIXME: add(ObjectModelFactory::create(*j));
                    }
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
  writer.write_raw("\n;; EOF ;;\n");
}

void
SectorModel::write(FileWriter& writer, const Gtk::TreeRow& row) const
{
  writer.start_section("layer");
  writer.write("name",   (Glib::ustring)(row[ObjectTreeColumns::instance().name]));
  writer.write("visible",          (bool)row[ObjectTreeColumns::instance().visible]);

  writer.start_section("objects");
  root_layer->write(writer);
  writer.end_section();

  writer.start_section("child-layers");
  for(Gtk::TreeStore::Children::iterator i = row.children().begin(); i != row.children().end(); ++i)
    {
      write(writer, *i);
    }
  writer.end_section();

  writer.end_section();
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
  std::cout << "ObjectTree:on_row_changed" << std::endl;
  if (iter)
    {
      // Update the Layer object with data from the tree
      HardLayerHandle layer = (*iter)[ObjectTreeColumns::instance().layer];
      if (layer.get())
        {
          layer->set_name(((Glib::ustring)(*iter)[ObjectTreeColumns::instance().name]).raw());
          layer->set_visible((*iter)[ObjectTreeColumns::instance().visible]);
          layer->set_locked((*iter)[ObjectTreeColumns::instance().locked]);
        }
    }
  queue_draw();
}

void
SectorModel::on_row_deleted(const Gtk::TreeModel::Path& path)
{
  std::cout << "ObjectTree:on_row_deleted" << std::endl;
  queue_draw();
}

void
SectorModel::on_row_has_child_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  std::cout << "ObjectTree:on_row_has_child_toggled" << std::endl;
}

void
SectorModel::on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  std::cout << "ObjectTree:on_row_inserted" << std::endl;
}

void
SectorModel::on_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order)
{
  std::cout << "ObjectTree:on_row_reordered" << std::endl;
}

/* EOF */
