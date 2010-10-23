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

#ifndef HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP

#include <boost/scoped_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

#include <gtkmm/treemodel.h>
#include <vector>
#include <iostream>
#include <gtkglmm.h>
#include <gdkmm/dragcontext.h>
#include <gtkmm/gl/widget.h>

#include "display/graphic_context_state.hpp"
#include "display/scene_context.hpp"
#include "display/texture.hpp"
#include "math/vector2f.hpp"
#include "editor/control_point.hpp"
#include "editor/decal_object_model.hpp"
#include "editor/selection.hpp"
#include "editor/select_mask.hpp"
#include "editor/layer.hpp"
#include "editor/command.hpp"
#include "editor/sector_model.hpp"
#include "editor/document.hpp"

class Tool;
class ScrollTool;
class SectorModel;
class EditorWindow;

/** OpenGL drawing area into which the Windstille game will be
    embedded */
class WindstilleWidget 
  : public Gtk::DrawingArea,
    public Gtk::GL::Widget<WindstilleWidget>
{
private:
  EditorWindow& editor;

  boost::scoped_ptr<Document>   m_document;
  boost::scoped_ptr<SceneGraph> m_scene_graph;
  bool m_rebuild_scene_graph;

  std::string filename;

  GraphicContextState   state;
  boost::scoped_ptr<Compositor> compositor;
  boost::scoped_ptr<SceneContext> sc;
  DecalObjectModel::MapType map_type;
  TexturePtr background_pattern;
  SelectMask select_mask;
  bool draw_background_pattern;
  bool draw_only_active_layers;
  bool grid_enabled;
  
public:
  WindstilleWidget(EditorWindow& editor,
                   const Glib::RefPtr<const Gdk::GL::Config>&  glconfig,
                   const Glib::RefPtr<const Gdk::GL::Context>& share_list);
  virtual ~WindstilleWidget();

  GraphicContextState& get_state() { return state; }

  virtual void on_realize();
  virtual bool on_timeout();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);

  virtual bool mouse_move(GdkEventMotion* event);
  virtual bool mouse_down (GdkEventButton* event);
  virtual bool mouse_up(GdkEventButton* event);
  virtual bool scroll(GdkEventScroll* event);

  virtual bool key_press(GdkEventKey* event);
  virtual bool key_release(GdkEventKey* event);

  // Drag&Drop
  virtual void on_drag_finish(const Glib::RefPtr<Gdk::DragContext>& context);
  virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>&, int x, int y,
                                     const Gtk::SelectionData& data, guint info, guint time);
  virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);

  void on_zoom_in();
  void on_zoom_out();
  void on_zoom_100();
  
  void draw();
  void update(float delta);

  Document&    get_document() const { return *m_document; }
  
  SelectMask& get_select_mask() { return select_mask; }

  SceneContext* get_sc() const { return sc.get(); }

  void load_file(const std::string& filename);

  void set_draw_background_pattern(bool v) { draw_background_pattern = v; }
  bool get_draw_background_pattern() const { return draw_background_pattern; }

  void set_draw_only_active_layer(bool v) { draw_only_active_layers = v; }
  bool get_draw_only_active_layer() const { return draw_only_active_layers; }
  void enable_grid(bool v) { grid_enabled = v; }
  bool get_enable_grid() const { return grid_enabled; }

  LayerHandle get_current_layer();
  Gtk::TreeModel::Path get_current_layer_path();

  std::string get_filename() const { return filename; }
  void set_filename(const std::string& filename_) { filename = filename_; }

  void save_screenshot(const std::string& filename);

private:
  void on_document_change();

private:
  WindstilleWidget (const WindstilleWidget&);
  WindstilleWidget& operator= (const WindstilleWidget&);
};

#endif

/* EOF */
