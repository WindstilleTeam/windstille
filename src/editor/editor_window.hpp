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

#ifndef HEADER_WINDSTILLE_EDITOR_EDITOR_WINDOW_HPP
#define HEADER_WINDSTILLE_EDITOR_EDITOR_WINDOW_HPP

#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/textview.h>
#include <gtkmm/paned.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/window.h>
#include <gtkmm/recentaction.h>
#include <gtkmm/notebook.h>
#include <gtkmm/radioaction.h>
#include <gdkmm/glcontext.h>

#include "util/currenton.hpp"
#include "editor/selection.hpp"
#include "editor/minimap_widget.hpp"
#include "editor/object_selector.hpp"
#include "editor/layer_manager.hpp"
#include "editor/document.hpp"

class AnimationWidget;
class Document;
class LayerWidget;
class NavgraphInsertTool;
class ScrollTool;
class SelectTool;
class TimelineWidget;
class Tool;
class WindstilleWidget;
class ZoomTool;

class EditorWindow : public Gtk::Window,
                     public Currenton<EditorWindow>
{
private:
  Gtk::VBox   vbox;
  Gtk::VBox   sidebar_vbox;
  Gtk::HBox   hbox;
  Gtk::HPaned hpaned;
  Gtk::VPaned vpaned;
  Gtk::HBox      status_hbox;
  Gtk::Label     status_label;
  Gtk::Statusbar statusbar;

  Glib::RefPtr<Gtk::UIManager>   ui_manager;
  Glib::RefPtr<Gtk::ActionGroup> action_group;
  Glib::RefPtr<Gdk::GLContext> share_list;

  Gtk::Notebook  notebook;
  ObjectSelector object_selector;
  LayerManager   layer_manager;
  MinimapWidget  minimap_widget;

  Glib::RefPtr<Gtk::RadioAction> select_tool_action;
  Glib::RefPtr<Gtk::RadioAction> navgraph_insert_tool_action;
  Glib::RefPtr<Gtk::RadioAction> navgraph_select_tool_action;
  Glib::RefPtr<Gtk::RadioAction> zoom_tool_action;

  Glib::RefPtr<Gtk::ToggleAction> toggle_color_layer;
  Glib::RefPtr<Gtk::ToggleAction> toggle_light_layer;
  Glib::RefPtr<Gtk::ToggleAction> toggle_highlight_layer;
  Glib::RefPtr<Gtk::ToggleAction> toggle_control_layer;

  Glib::RefPtr<Gtk::ToggleAction> background_layer;
  Glib::RefPtr<Gtk::ToggleAction> visible_layer;
  Glib::RefPtr<Gtk::ToggleAction> grid_layer;

  Glib::RefPtr<Gtk::ToggleAction> play_action;
  Glib::RefPtr<Gtk::ToggleAction> snap_action;

  std::unique_ptr<SelectTool>   select_tool;
  std::unique_ptr<NavgraphInsertTool> navgraph_insert_tool;
  std::unique_ptr<ZoomTool>     zoom_tool;
  std::unique_ptr<ScrollTool>   scroll_tool;
  Tool* current_tool;
  LayerWidget* layer_widget;

  sigc::connection  timeout_connection;
  SelectionHandle   clipboard;

public:
  EditorWindow();
  ~EditorWindow() override;

  void show_minimap(bool v);

public:
  void on_new();
  void on_open();
  void on_save();
  void on_save_as();
  void on_save_screenshot();
  void on_close();

  bool on_timeout();
  void on_play();

  void on_about_clicked();
  void on_quit();

  void on_zoom_in();
  void on_zoom_out();
  void on_zoom_100();

  void on_undo();
  void on_redo();

  void update_undo_state();

  void on_cut();
  void on_copy();
  void on_paste();

  void on_layer_toggle(int layer, bool status);

  void on_switch_page(Gtk::Widget* page, guint page_num);
  void on_tool_select(Glib::RefPtr<Gtk::RadioAction> action, Tool*);

  void on_select_all();

  /** Animation Callbacks
   * @{*/
  void on_animation_new();
  void on_animation_export();
  void on_animation_delete();

  void on_animation_frame_forward();
  void on_animation_frame_backward();

  void on_animation_layer_new();

  void on_animation_add_keyframe(TimelineProperty property);
  /** @} */

  void toggle_render_layer(Glib::RefPtr<Gtk::ToggleAction> action, unsigned int mask);
  void toggle_background_layer(Glib::RefPtr<Gtk::ToggleAction> action);
  void toggle_draw_only_active_layer(Glib::RefPtr<Gtk::ToggleAction> action);
  void toggle_grid_layer(Glib::RefPtr<Gtk::ToggleAction> action);

  void on_new_layer();
  void on_delete_layer();
  void on_reverse_layers();

  Tool* get_current_tool() const;
  ScrollTool* get_scroll_tool() const;

  LayerManager& get_layer_manager() { return layer_manager; }
  WindstilleWidget* get_windstille_widget();
  TimelineWidget*   get_timeline_widget();
  AnimationWidget*   get_animation_widget();
  Document* get_document();

  void load_file(const std::string& filename);

  /** Queue a file to be loaded once the editor is full realized */
  void queue_for_load(const std::string& filename);

  void call_with_document(void (Document::*func)());

  void on_recent_file(const Glib::RefPtr<Gtk::RecentAction>& recent_action);

  void on_show_all(bool v);
  void on_lock_all(bool v);

  void add_recent_file(const std::string& filename);

  void print(const std::string& text);
  bool remove_message(guint id);

  void print_coordinates(const std::string& text);

  Glib::RefPtr<Gtk::UIManager>   get_ui_manager() const { return ui_manager; }
  Glib::RefPtr<Gtk::ActionGroup> get_action_group() const { return action_group; }

private:
  EditorWindow(const EditorWindow&);
  EditorWindow& operator=(const EditorWindow&);
};

#endif

/* EOF */
