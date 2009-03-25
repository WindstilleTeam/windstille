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

#include <iostream>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/stock.h>

#include "display/scene_context.hpp"
#include "windstille_widget.hpp"
#include "about_window.hpp"
#include "editor_window.hpp"
#include "zoom_tool.hpp"
#include "select_tool.hpp"

EditorWindow* EditorWindow::current_ = 0;

EditorWindow::EditorWindow(const Glib::RefPtr<const Gdk::GL::Config>& glconfig_)
  : minimap_widget(glconfig_),
    glconfig(glconfig_),
    select_tool(new SelectTool()),
    zoom_tool(new ZoomTool()),
    current_tool(select_tool.get())
{
  current_ = this;

  set_title("Windstille Editor");
  set_default_size(1024, 768);

  Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "      <menuitem action='Open'/>"
    "      <menuitem action='Save'/>"
    "      <menuitem action='Close'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='MenuEdit'>"
    "      <menuitem action='Cut'/>"
    "      <menuitem action='Copy'/>"
    "      <menuitem action='Paste'/>"
    "      <separator/>"
    "      <menuitem action='Delete'/>"
    "      <menuitem action='Duplicate'/>"
    "    </menu>"
    "    <menu action='MenuObject'>"
    "      <menuitem action='RaiseObjectToTop'/>"
    "      <menuitem action='RaiseObject'/>"
    "      <menuitem action='LowerObject'/>"
    "      <menuitem action='LowerObjectToBottom'/>"
    "      <separator/>"
    "      <menuitem action='ConnectParent'/>"
    "      <menuitem action='ClearParent'/>"
    "      <separator/>"
    //"      <menuitem action='Snap'/>"
    "    </menu>"
    "    <menu action='MenuView'>"
    "      <menuitem action='ZoomIn'/>"
    "      <menuitem action='ZoomOut'/>"
    "      <menuitem action='Zoom100'/>"
    "      <separator/>"
    "      <menuitem action='ToggleColorLayer'/>"
    "      <menuitem action='ToggleLightLayer'/>"
    "      <menuitem action='ToggleHighlightLayer'/>"
    "      <menuitem action='ToggleControlLayer'/>"
    "    </menu>"
    "    <menu action='MenuTools'>"
    "      <menuitem action='SelectTool'/>"
    "      <menuitem action='NodeTool'/>"
    "      <menuitem action='ZoomTool'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    ""
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='New'/>"
    "    <toolitem action='Open'/>"
    "    <toolitem action='Save'/>"
    "    <separator/>"
    "    <toolitem action='Undo'/>"
    "    <toolitem action='Redo'/>"
    "    <separator/>"
    "    <toolitem action='Cut'/>"
    "    <toolitem action='Copy'/>"
    "    <toolitem action='Paste'/>"
    "    <separator/>"
    "    <toolitem action='Delete'/>"
    "    <toolitem action='Duplicate'/>"
    "    <separator/>"
    "    <toolitem action='ZoomIn'/>"
    "    <toolitem action='ZoomOut'/>"
    "    <toolitem action='Zoom100'/>"
    "    <separator/>"
    "    <toolitem action='LowerObjectToBottom'/>"
    "    <toolitem action='LowerObject'/>"
    "    <toolitem action='RaiseObject'/>"
    "    <toolitem action='RaiseObjectToTop'/>"
    "    <separator/>"
    "    <toolitem action='ConnectParent'/>"
    "    <toolitem action='ClearParent'/>"
    "    <separator/>"
    //"    <toolitem action='Snap'/>"
    "    <toolitem action='Play'/>"
    "  </toolbar>"
    ""
    "  <toolbar  name='ToolBox'>"
    "    <toolitem action='SelectTool'/>"
    "    <toolitem action='NodeTool'/>"
    "    <toolitem action='ZoomTool'/>"
    "    <separator/>"
    "    <toolitem action='ToggleColorLayer'/>"
    "    <toolitem action='ToggleLightLayer'/>"
    "    <toolitem action='ToggleHighlightLayer'/>"
    "    <toolitem action='ToggleControlLayer'/>"
    "  </toolbar>"
    "</ui>";

  ui_manager   = Gtk::UIManager::create();
  action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("MenuFile",    "_File"));
  action_group->add(Gtk::Action::create("New",         Gtk::Stock::NEW),
                    sigc::mem_fun(*this, &EditorWindow::on_new));
  action_group->add(Gtk::Action::create("Open",        Gtk::Stock::OPEN),
                    sigc::mem_fun(*this, &EditorWindow::on_open));
  action_group->add(Gtk::Action::create("Save",        Gtk::Stock::SAVE),
                    sigc::mem_fun(*this, &EditorWindow::on_save));
  action_group->add(Gtk::Action::create("Close",       Gtk::Stock::CLOSE),
                    sigc::mem_fun(*this, &EditorWindow::on_close));
  action_group->add(Gtk::Action::create("Quit",        Gtk::Stock::QUIT),
                    sigc::mem_fun(*this, &EditorWindow::on_quit));

  action_group->add(Gtk::Action::create("MenuEdit",    "_Edit"));
  action_group->add(Gtk::Action::create("Undo",        Gtk::Stock::UNDO));
  action_group->add(Gtk::Action::create("Redo",        Gtk::Stock::REDO));
  action_group->add(Gtk::Action::create("Cut",         Gtk::Stock::CUT));
  action_group->add(Gtk::Action::create("Copy",        Gtk::Stock::COPY));
  action_group->add(Gtk::Action::create("Paste",       Gtk::Stock::PASTE));

  action_group->add(Gtk::Action::create("Delete",      Gtk::Stock::DELETE),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_delete));
  action_group->add(Gtk::Action::create_with_icon_name("Duplicate", "duplicate", "Duplicate Object", "Duplicate Object"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_duplicate));

  action_group->add(Gtk::Action::create("MenuObject",    "_Object"));
  action_group->add(Gtk::Action::create_with_icon_name("RaiseObjectToTop", "object_raise_to_top", "Raise To Top", "Raise Object to Top"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_raise_to_top));
  action_group->add(Gtk::Action::create_with_icon_name("RaiseObject", "object_raise", "Raise", "Raise Object"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_raise));
  action_group->add(Gtk::Action::create_with_icon_name("LowerObject", "object_lower", "Lower", "Lower Object"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_lower));                    
  action_group->add(Gtk::Action::create_with_icon_name("LowerObjectToBottom", "object_lower_to_bottom", "Lower To Bottom", "Lower Object to Bottom"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_lower_to_bottom));

  action_group->add(Gtk::Action::create_with_icon_name("ConnectParent", "connect_parent", "Connect Parent", "Connect Parent"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_connect_parent));
  action_group->add(Gtk::Action::create_with_icon_name("ClearParent", "clear_parent", "Clear Parent", "Clear Parent"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_clear_parent));


  action_group->add(Gtk::Action::create("MenuView",    "_View"));
  action_group->add(Gtk::Action::create("Zoom100",     Gtk::Stock::ZOOM_100),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_100));
  action_group->add(Gtk::Action::create("ZoomIn",      Gtk::Stock::ZOOM_IN),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_in));
  action_group->add(Gtk::Action::create("ZoomOut",     Gtk::Stock::ZOOM_OUT),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_out));
  action_group->add(play_action = Gtk::ToggleAction::create("Play", Gtk::Stock::MEDIA_PLAY), 
                    sigc::mem_fun(*this, &EditorWindow::on_play));
  action_group->add(snap_action = Gtk::ToggleAction::create("Snap", Gtk::Stock::MEDIA_PAUSE),
                    sigc::mem_fun(*this, &EditorWindow::on_snap));

  action_group->add(Gtk::Action::create("MenuHelp",    "_Help"));
  action_group->add(Gtk::Action::create("About",       Gtk::Stock::ABOUT),
                    sigc::mem_fun(*this, &EditorWindow::on_about_clicked));

  Glib::RefPtr<Gtk::ToggleAction> toggle_color_layer = Gtk::ToggleAction::create_with_icon_name("ToggleColorLayer", "color", "Toogle Color Layer", "Toogle Color Layer");
  Glib::RefPtr<Gtk::ToggleAction> toggle_light_layer = Gtk::ToggleAction::create_with_icon_name("ToggleLightLayer", "light", "Toogle Light Layer", "Toogle Light Layer");
  Glib::RefPtr<Gtk::ToggleAction> toggle_highlight_layer = Gtk::ToggleAction::create_with_icon_name("ToggleHighlightLayer", "highlight", "Toogle Highlight Layer", "Toogle Highlight Layer");
  Glib::RefPtr<Gtk::ToggleAction> toggle_control_layer = Gtk::ToggleAction::create_with_icon_name("ToggleControlLayer", "control", "Toogle Control Layer", "Toogle Control Layer");
  
  toggle_color_layer->set_active(true);
  toggle_light_layer->set_active(true);
  toggle_highlight_layer->set_active(true);
  toggle_control_layer->set_active(true);

  action_group->add(toggle_color_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_color_layer, (uint32_t)SceneContext::COLORMAP));
  action_group->add(toggle_light_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_light_layer, (uint32_t)SceneContext::LIGHTMAP));
  action_group->add(toggle_highlight_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_highlight_layer, (uint32_t)SceneContext::HIGHLIGHTMAP));
  action_group->add(toggle_control_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_control_layer, (uint32_t)SceneContext::CONTROLMAP));

  // Tools
  action_group->add(Gtk::Action::create("MenuTools",  "_Tools"));
  Gtk::RadioButtonGroup tool_group;
  
  select_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "SelectTool", "select_tool", "Select Tool", "Select Tool");
  node_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "NodeTool",   "node_tool",   "Node Tool", "Node Tool");
  zoom_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "ZoomTool",   "zoom_tool",   "Zoom Tool", "Zoom Tool");

  action_group->add(select_tool_action, sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), select_tool_action, select_tool.get()));
  action_group->add(node_tool_action,   sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), node_tool_action, select_tool.get()));
  action_group->add(zoom_tool_action,   sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), zoom_tool_action, zoom_tool.get()));

  // signal_size_allocate().connect (sigc::mem_fun (*this, &EditorWindow::on_window_size_allocate), false);
  // signal_realize().connect (sigc::mem_fun (*this, &EditorWindow::on_window_realize));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string(ui_info);

  add_accel_group(ui_manager->get_accel_group());

  notebook.signal_switch_page().connect(sigc::mem_fun(*this, &EditorWindow::on_switch_page));

  // Disable unimplemented stuff:
  action_group->get_action("Undo")->set_sensitive(false);
  action_group->get_action("Redo")->set_sensitive(false);
  action_group->get_action("Cut")->set_sensitive(false);
  action_group->get_action("Copy")->set_sensitive(false);
  action_group->get_action("Paste")->set_sensitive(false);

  // Packing

  // Main Vbox
  vbox.pack_start(*ui_manager->get_widget("/MenuBar"), Gtk::PACK_SHRINK);
  vbox.pack_start(*ui_manager->get_widget("/ToolBar"), Gtk::PACK_SHRINK);
  vbox.add(hbox);
  vbox.pack_end(status, Gtk::PACK_SHRINK);

  // Hbox
  hbox.pack_start(*ui_manager->get_widget("/ToolBox"), Gtk::PACK_SHRINK);
  dynamic_cast<Gtk::Toolbar*>(ui_manager->get_widget("/ToolBox"))->set_orientation(Gtk::ORIENTATION_VERTICAL);
  hbox.add(hpaned);

  // vpaned.set_size_request(250, -1);
  //object_selector.set_size_request(250, 300);
  //object_tree.set_size_request(250, 300);
  
  hpaned.pack1(notebook,     Gtk::EXPAND);
  hpaned.pack2(sidebar_vbox, Gtk::SHRINK);
 
  sidebar_vbox.pack_start(vpaned, Gtk::PACK_EXPAND_WIDGET);
  sidebar_vbox.pack_start(minimap_widget, Gtk::PACK_SHRINK);
  
  vpaned.pack1(object_selector, Gtk::EXPAND);
  vpaned.pack2(object_tree,     Gtk::SHRINK);

  hpaned.set_position(730);
  vpaned.set_position(420);

  // Window
  add(vbox);

  fill_object_selector("data/images/objects/");
  fill_object_selector("data/images/inventory/");
  fill_object_selector("data/images/portraits/");
  fill_object_selector("data/images/");
}

EditorWindow::~EditorWindow()
{
  current_ = 0;
}

void
EditorWindow::on_about_clicked()
{
  AboutWindow about_dialog;
  about_dialog.run();
}

void
EditorWindow::on_quit()
{
  hide();
}

void
EditorWindow::on_new()
{
  std::cout << "on_new" << std::endl;

  // FIXME: We abuse the minimap as our root GLContext
  WindstilleWidget* windstille = new WindstilleWidget(glconfig, minimap_widget.get_gl_context());

  Glib::ustring title = Glib::ustring::compose("Sector %1", notebook.get_n_pages());
  int new_page = notebook.append_page(*windstille, title);
  windstille->show();
  notebook.set_current_page(new_page); 

  object_tree.set_model(windstille->get_sector_model());
}

void
EditorWindow::on_open()
{
  Gtk::FileChooserDialog dialog("Open File",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN,   Gtk::RESPONSE_OK);

  switch(dialog.run())
    {
      case(Gtk::RESPONSE_OK):
        {
          std::cout << "Select clicked." << std::endl;
          std::cout << "Folder selected: " << dialog.get_filename()
                    << std::endl;
          break;
        }

      case(Gtk::RESPONSE_CANCEL):
        {
          std::cout << "Cancel clicked." << std::endl;
          break;
        }
    }
}

void
EditorWindow::on_save()
{
  Gtk::FileChooserDialog dialog("Save File",
                                Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE,   Gtk::RESPONSE_OK);

  switch(dialog.run())
    {
      case(Gtk::RESPONSE_OK):
        {
          std::cout << "Select clicked." << std::endl;
          std::cout << "Folder selected: " << dialog.get_filename()
                    << std::endl;
          break;
        }

      case(Gtk::RESPONSE_CANCEL):
        {
          std::cout << "Cancel clicked." << std::endl;
          break;
        }
    }
}

void
EditorWindow::on_close()
{
  int page = notebook.get_current_page();
  if (page != -1)
    {
      notebook.remove_page(page);

      if (!get_windstille_widget())
        object_tree.set_model(0);
    }
}

void
EditorWindow::show_minimap(bool v)
{
  if (v)
    {
      minimap_widget.show();
    }
  else
    {
      minimap_widget.hide();
    }
}

void
EditorWindow::on_zoom_in()
{
  WindstilleWidget* wst = get_windstille_widget();
  if (wst)
    wst->on_zoom_in();
}

void
EditorWindow::on_zoom_out()
{
  WindstilleWidget* wst = get_windstille_widget();
  if (wst)
    wst->on_zoom_out();
}

void
EditorWindow::on_zoom_100()
{
  WindstilleWidget* wst = get_windstille_widget();
  if (wst)
    wst->on_zoom_100();
}

void
EditorWindow::on_tool_select(Glib::RefPtr<Gtk::RadioAction> action, Tool* tool)
{
  std::cout << "on_tool_select()" << action->get_active() << std::endl;
  if (action->get_active())
    {
      current_tool = tool;
    }
}

void
EditorWindow::toggle_render_layer(Glib::RefPtr<Gtk::ToggleAction> action, uint32_t mask)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      SceneContext& sc = *wst->get_sc();

      if (action->get_active())
        {
          sc.set_render_mask(sc.get_render_mask() | mask);
        }
      else
        {
          sc.set_render_mask(sc.get_render_mask() & (~mask));
        }

      std::cout << "mask: " << sc.get_render_mask() << std::endl;
      wst->queue_draw();
    }
}

Tool*
EditorWindow::get_current_tool() const
{
  return current_tool;
}

WindstilleWidget*
EditorWindow::get_windstille_widget()
{
  int page = notebook.get_current_page();
  if (page == -1)
    {
      return 0;
    }
  else
    {
      return static_cast<WindstilleWidget*>(notebook.get_nth_page(page));
    }
}

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

void
EditorWindow::fill_object_selector(const std::string& directory)
{
  Glib::Dir dir(directory);
  for(Glib::Dir::iterator i = dir.begin(); i != dir.end(); ++i)
    {
      if (has_suffix(*i, ".png"))
        {
          Glib::ustring path = directory;
          path += *i;

          Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(path);

          object_selector.add_object(path, pixbuf->scale_simple(32, 32, Gdk::INTERP_TILES));
        }
    }
}

void
EditorWindow::on_switch_page(GtkNotebookPage* page, guint page_num)
{
  std::cout << "on_switch_page(" << page << ", " << page_num << ")" << std::endl;
  if (get_windstille_widget())
    object_tree.set_model(get_windstille_widget()->get_sector_model());
  else
    object_tree.set_model(0);
}

void
EditorWindow::call_with_windstille_widget(void (WindstilleWidget::*func)())
{
  WindstilleWidget* wst = get_windstille_widget();
  if (wst)
    {
      (wst->*func)();
    }
}

bool
EditorWindow::on_timeout()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->update(0.050f);
    }
  return true;
}

void
EditorWindow::on_play()
{
  if (play_action->get_active())
    {
      std::cout << "Play" << std::endl;
      timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &EditorWindow::on_timeout),
                                                          50,
                                                          Glib::PRIORITY_DEFAULT);
    }
  else
    {
      std::cout << "Stop" << std::endl;
      timeout_connection.disconnect();
    }
}

void
EditorWindow::on_snap()
{
  if (snap_action->get_active())
    {
      std::cout << "Snap" << std::endl;
    }
  else
    {
      std::cout << "Snap Off" << std::endl;
    }
}

/* EOF */
