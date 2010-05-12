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
#include <algorithm>
#include <gdkmm/pixbuf.h>

#include "display/scene_context.hpp"
#include "display/surface.hpp"
#include "editor/editor_window.hpp"
#include "editor/layer_manager_columns.hpp"
#include "editor/navgraph_edge_object_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "editor/navigation_graph_model.hpp"
#include "editor/object_model_factory.hpp"
#include "editor/sector_model.hpp"
#include "editor/sector_model_builder.hpp"
#include "editor/timeline.hpp"
#include "editor/windstille_widget.hpp"
#include "navigation/node.hpp"
#include "scenegraph/drawable_group.hpp"
#include "util/file_reader.hpp"
#include "util/pathname.hpp"

LayerManagerColumns* LayerManagerColumns::instance_ = 0;

SectorModel::SectorModel(const std::string& filename) :
  nav_graph(new NavigationGraphModel(*this)),
  layer_tree(Gtk::ListStore::create(LayerManagerColumns::instance())),
  m_timeline(new Timeline()),
  ambient_color()
{
  register_callbacks();
  SectorModelBuilder(filename, *this);
}

SectorModel::SectorModel()
  : nav_graph(new NavigationGraphModel(*this)),
    layer_tree(Gtk::ListStore::create(LayerManagerColumns::instance())),
    m_timeline(new Timeline()),
    ambient_color()
{
  register_callbacks();

  Gtk::ListStore::iterator it = layer_tree->append();

  LayerHandle layer(new Layer(*this));
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file(Pathname("editor/type.png", Pathname::kDataPath).get_sys_path());
  (*it)[LayerManagerColumns::instance().name]      = Glib::ustring("Scene");
  (*it)[LayerManagerColumns::instance().visible]   = true;
  (*it)[LayerManagerColumns::instance().locked]    = false;
  (*it)[LayerManagerColumns::instance().layer]     = layer;
  layer->sync(*it);
}

SectorModel::~SectorModel()
{
}

void
SectorModel::register_callbacks()
{  
  layer_tree->signal_row_changed().connect(sigc::mem_fun(*this, &SectorModel::on_row_changed));
  layer_tree->signal_row_deleted().connect(sigc::mem_fun(*this, &SectorModel::on_row_deleted));
  layer_tree->signal_row_has_child_toggled().connect(sigc::mem_fun(*this, &SectorModel::on_row_has_child_toggled));
  layer_tree->signal_row_inserted().connect(sigc::mem_fun(*this, &SectorModel::on_row_inserted));
  layer_tree->signal_rows_reordered().connect(sigc::mem_fun(*this, &SectorModel::on_rows_reordered));
}

void
SectorModel::add_layer(LayerHandle layer, const Gtk::TreeModel::Path& path)
{
  Gtk::ListStore::iterator it;

  if (path.empty())
    it = layer_tree->append();
  else
    it = layer_tree->insert(layer_tree->get_iter(path));
  
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file(Pathname("editor/type.png", Pathname::kDataPath).get_sys_path());
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

  LayerHandle layer(new Layer(*this));
  (*it)[LayerManagerColumns::instance().type_icon] = Gdk::Pixbuf::create_from_file(Pathname("/editor/type.png", Pathname::kDataPath).get_sys_path());
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
SectorModel::reverse_layers()
{
  std::vector<int> reverse_order;

  size_t size = layer_tree->children().size();
  for(int i = size-1; i >= 0; --i)
    reverse_order.push_back(i);

  layer_tree->reorder(reverse_order);
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
    static_cast<LayerHandle>((*it)[LayerManagerColumns::instance().layer])->add(object);
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

SectorModel::Layers
SectorModel::get_layers() const
{
  Layers lst;
 
  // LayerTree holds the layers in reverse order, so we reverse them here
  Gtk::TreeModel::Children childs = layer_tree->children(); 
  for(Gtk::TreeModel::Children::const_iterator it = childs.begin();
      it != childs.end(); ++it)
  {
    lst.push_back((*it)[LayerManagerColumns::instance().layer]);
  }

  // FIXME: for some reason reverse iterators don't work with
  // Gtk::TreeModel::Children, so we do it manually
  std::reverse(lst.begin(), lst.end());

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
SectorModel::draw(SceneContext& sc, const SelectMask& layermask)
{
  // Draw Layers
  const Layers& layers = get_layers();
 
  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
  {
    if ((*i)->is_visible())
      (*i)->draw(sc, layermask);
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
  
  if (ObjectModelHandle obj = nav_graph->get_object_at(pos, layermask))
  {
    return obj;
  }

  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
  {
    if ((*i)->is_visible() && !(*i)->is_locked())
    {
      ObjectModelHandle object = (*i)->get_object_at(pos, layermask);
          
      if (object)
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

  {
    SelectionHandle new_sel = nav_graph->get_selection(rect, layermask);
    selection->add(new_sel->begin(), new_sel->end());
  }

  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
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
  for(Layers::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
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
SectorModel::snap_object(ObjectModelHandle object, const std::set<ObjectModelHandle>& ignore_objects) const
{
  const Layers& layers = get_layers();

  SnapData snap_data;
  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
  {
    if ((*i)->is_visible())
    {
      snap_data.merge((*i)->snap_object(object, ignore_objects));
    }
  }

  return snap_data;
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

  writer.start_section("timeline");
  m_timeline->write(writer);
  writer.end_section();

  writer.start_section("navigation");
  nav_graph->write(writer);
  writer.end_section();

  writer.start_section("layers");
  const Layers& layers = get_layers();
  for(Layers::const_iterator i = layers.begin(); i != layers.end(); ++i)
  {
    writer.start_section("layer");
    writer.write("name",    (*i)->get_name());
    writer.write("visible", (*i)->is_visible());
    writer.write("locked",  (*i)->is_locked());

    writer.start_section("objects");
    (*i)->write(writer);
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

  PropSetFunctor(bool v_) :
    v(v_)
  {}

  bool set_visible(const Gtk::TreeModel::iterator& it)
  {
    (*it)[LayerManagerColumns::instance().visible] = v;
    static_cast<LayerHandle>((*it)[LayerManagerColumns::instance().layer])->sync(*it);
    return false;
  }
  
  bool set_locked(const Gtk::TreeModel::iterator& it)
  {
    (*it)[LayerManagerColumns::instance().locked] = v;
    static_cast<LayerHandle>((*it)[LayerManagerColumns::instance().layer])->sync(*it);
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
SectorModel::rebuild_scene_graph(DrawableGroup& sg)
{
  // FIXME: should make a queue_rebuild_scene_graph() to limit the number of rebuilds per frame to 1
  sg.clear();
  
  const Layers& layers = get_layers();
  for(Layers::const_iterator layer = layers.begin(); layer != layers.end(); ++layer)
  {
    if (*layer)
    {
      boost::shared_ptr<DrawableGroup> group(new DrawableGroup);

      for(Layer::const_iterator obj = (*layer)->begin(); obj != (*layer)->end(); ++obj)
      {
        if ((*layer)->is_visible())
        {
          (*obj)->add_to_scenegraph(*group);
        }
      }

      sg.add_drawable(group);
    }
  }

  //std::cout << "rebuild_scene_graph: " << nav_graph->get_nodes().size() << std::endl;
  for(NavigationGraphModel::Nodes::const_iterator i = nav_graph->get_nodes().begin(); i != nav_graph->get_nodes().end(); ++i)
  {
    (*i)->add_to_scenegraph(sg);
  }
}

void
SectorModel::on_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
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
}

void
SectorModel::on_row_deleted(const Gtk::TreeModel::Path& path)
{
  std::cout << "LayerManager:on_row_deleted" << std::endl;
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

void
SectorModel::delete_navgraph_edges(NavGraphNodeObjectModel& node)
{
  // FIXME: Kind of ugly, higher level template might help
  const Layers& layers = get_layers();
  for(Layers::const_reverse_iterator layer = layers.rbegin(); layer != layers.rend(); ++layer)
  {
    if (*layer)
    {
      for(Layer::const_iterator obj = (*layer)->begin(); obj != (*layer)->end(); ++obj)
      {
        boost::shared_ptr<NavGraphEdgeObjectModel> edge = boost::dynamic_pointer_cast<NavGraphEdgeObjectModel>(*obj);
        if (edge)
        {
          /*
            if (edge.get_lhs().get() == &node ||
            edge.get_rhs().get() == &node)
            {

            }
          */
        }
      }
    }
  }  
}

/* EOF */
