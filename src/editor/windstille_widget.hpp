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

#ifndef HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP

#include <iostream>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <iostream>

#include <gtkmm/treemodel.h>
#include <gtkmm/glarea.h>
#include <gdkmm/dragcontext.h>
#include <glm/glm.hpp>

#include <wstdisplay/graphic_context_state.hpp>
#include <wstdisplay/scene_context.hpp>
#include <wstdisplay/texture.hpp>
#include <wstdisplay/graphics_context.hpp>
#include "editor/control_point.hpp"
#include "editor/decal_object_model.hpp"
#include "editor/selection.hpp"
#include "editor/select_mask.hpp"
#include "editor/layer.hpp"
#include "editor/command.hpp"
#include "editor/sector_model.hpp"
#include "editor/document.hpp"

namespace windstille {


class Tool;
class ScrollTool;
class SectorModel;
class EditorWindow;


/** OpenGL drawing area into which the Windstille game will be
    embedded */
class WindstilleWidget final : public Gtk::GLArea
{
private:
  EditorWindow& m_editor;
  std::unique_ptr<wstdisplay::GraphicsContext> m_gc;

  std::unique_ptr<Document>   m_document;
  std::unique_ptr<wstdisplay::SceneGraph> m_scene_graph;
  bool m_rebuild_scene_graph;

  std::string filename;

  wstdisplay::GraphicContextState   state;
  std::unique_ptr<wstdisplay::Compositor> compositor;
  std::unique_ptr<wstdisplay::SceneContext> sc;
  DecalObjectModel::MapType map_type;
  wstdisplay::TexturePtr background_pattern;
  SelectMask select_mask;
  bool draw_background_pattern;
  bool draw_only_active_layers;
  bool grid_enabled;

public:
  WindstilleWidget(EditorWindow& editor);
  ~WindstilleWidget() override;

  wstdisplay::GraphicContextState& get_state() { return state; }
  Glib::RefPtr<Gdk::GLContext> on_create_context() override;
  void on_realize() override;
  void on_unrealize() override;
  bool on_render(Glib::RefPtr<Gdk::GLContext> const& context) override;
  void on_resize(int width, int height) override;

  bool mouse_move(GdkEventMotion* event);
  bool mouse_down (GdkEventButton* event);
  bool mouse_up(GdkEventButton* event);
  bool scroll(GdkEventScroll* event);

  bool key_press(GdkEventKey* event);
  bool key_release(GdkEventKey* event);

  // Drag&Drop
  void on_drag_data_received(Glib::RefPtr<Gdk::DragContext> const&, int x, int y,
                             Gtk::SelectionData const& data, guint info, guint time) override;
  bool on_drag_drop(Glib::RefPtr<Gdk::DragContext> const& context, int x, int y, guint time) override;
  void on_drag_end(Glib::RefPtr<Gdk::DragContext> const& context) override;

  void on_zoom_in();
  void on_zoom_out();
  void on_zoom_100();

  void draw(wstdisplay::GraphicsContext& gc);
  void update(float delta);

  Document&    get_document() const { return *m_document; }

  SelectMask& get_select_mask() { return select_mask; }

  wstdisplay::SceneContext* get_sc() const { return sc.get(); }

  void load_file(std::string const& filename);

  void set_draw_background_pattern(bool v) { draw_background_pattern = v; }
  bool get_draw_background_pattern() const { return draw_background_pattern; }

  void set_draw_only_active_layer(bool v) { draw_only_active_layers = v; }
  bool get_draw_only_active_layer() const { return draw_only_active_layers; }
  void enable_grid(bool v) { grid_enabled = v; }
  bool get_enable_grid() const { return grid_enabled; }

  LayerHandle get_current_layer();
  Gtk::TreeModel::Path get_current_layer_path();

  std::string get_filename() const { return filename; }
  void set_filename(std::string const& filename_) { filename = filename_; }

  void save_screenshot(std::string const& filename);

private:
  void on_document_change();

private:
  WindstilleWidget (WindstilleWidget const&);
  WindstilleWidget& operator= (WindstilleWidget const&);
};


} // namespace windstille

#endif

/* EOF */
