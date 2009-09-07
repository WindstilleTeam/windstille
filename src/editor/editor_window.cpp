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

// /usr/include/gtkmm-2.4/gtkmm/recentmanager.h causes warning, so we have to disable -Weffc++
#pragma GCC diagnostic ignored "-Weffc++"

#include <fstream>
#include <iostream>
#include <gdkmm/pixbuf.h>
#include <glibmm/miscutils.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/recentaction.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/stock.h>
#include <gtkmm/separatortoolitem.h>

#include "editor/layer_commands.hpp"
#include "editor/undo_manager.hpp"
#include "display/scene_context.hpp"
#include "editor/windstille_widget.hpp"
#include "editor/about_window.hpp"
#include "editor/editor_window.hpp"
#include "editor/zoom_tool.hpp"
#include "editor/select_tool.hpp"
#include "editor/navgraph_insert_tool.hpp"
#include "editor/sector_model.hpp"
#include "editor/layer_widget.hpp"

#include "editor/editor_window.hpp"

EditorWindow::EditorWindow(const Glib::RefPtr<const Gdk::GL::Config>& glconfig_)
  : vbox(),
    sidebar_vbox(),
    hbox(),
    hpaned(),
    vpaned(),
    status(),
    ui_manager(Gtk::UIManager::create()),
    action_group(Gtk::ActionGroup::create()),
    share_list(),
    glconfig(glconfig_),
    notebook(),
    object_selector(*this),
    layer_manager(*this),
    minimap_widget(glconfig_),
    select_tool_action(),
    navgraph_insert_tool_action(),
    navgraph_select_tool_action(),
    zoom_tool_action(),
    toggle_color_layer(),
    toggle_light_layer(),
    toggle_highlight_layer(),
    toggle_control_layer(),
    background_layer(),
    visible_layer(),
    grid_layer(),
    play_action(),
    snap_action(),
    select_tool(new SelectTool()),
    navgraph_insert_tool(new NavgraphInsertTool()),
    zoom_tool(new ZoomTool()),
    current_tool(select_tool.get()),
    layer_widget(),
    timeout_connection(),
    clipboard()
{
  set_title("Windstille Editor");
  set_default_size(1280, 800);

  Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "      <menuitem action='FileOpen'/>"
    // FIXME: This doesn't work, RecentActon seems to create both an
    // Open entry, as well as a menu-button, with the toolbar this
    // works fine, with the menu it doesn't as both entries are merged
    // into one, activating the callback on hover
    //"      <menuitem action='FileRecentFiles'/>"
    "      <separator/>"
    "      <menuitem action='Save'/>"
    "      <menuitem action='SaveAs'/>"
    "      <separator/>"
    "      <menuitem action='SaveScreenshot'/>"
    "      <separator/>"
    "      <menuitem action='Close'/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='MenuEdit'>"
    "      <menuitem action='Cut'/>"
    "      <menuitem action='Copy'/>"
    "      <menuitem action='Paste'/>"
    "      <separator/>"
    "      <menuitem action='Duplicate'/>"
    "      <menuitem action='Delete'/>"
    "      <separator/>"
    "      <menuitem action='SelectAll'/>"
    "    </menu>"
    "    <menu action='MenuObject'>"
    "      <menuitem action='RaiseObjectToTop'/>"
    "      <menuitem action='RaiseObject'/>"
    "      <menuitem action='LowerObject'/>"
    "      <menuitem action='LowerObjectToBottom'/>"
    "      <separator/>"
    "      <menuitem action='HFlipObject'/>"
    "      <menuitem action='VFlipObject'/>"
    "      <separator/>"
    "      <menuitem action='ConnectParent'/>"
    "      <menuitem action='ClearParent'/>"
    "      <separator/>"
    //"      <menuitem action='Snap'/>"
    "    </menu>"
    "    <menu action='MenuLayer'>"
    "      <menuitem action='NewLayer'/>"
    "      <menuitem action='DeleteLayer'/>"
    "      <menuitem action='ReverseLayers'/>"
    "      <separator/>"
    "      <menuitem action='ShowAllLayer'/>"
    "      <menuitem action='HideAllLayer'/>"
    "      <separator/>"
    "      <menuitem action='LockAllLayer'/>"
    "      <menuitem action='UnlockAllLayer'/>"
    "      <separator/>"
    "      <menuitem action='AutoLockLayer'/>"
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
    "      <separator/>"
    "      <menuitem action='ToggleBackgroundLayer'/>"
    "      <menuitem action='ToggleVisibleLayer'/>"
    "      <menuitem action='ToggleGridLayer'/>"
    "    </menu>"
    "    <menu action='MenuTools'>"
    "      <menuitem action='SelectTool'/>"
    "      <menuitem action='NavgraphInsertTool'/>"
    "      <menuitem action='ZoomTool'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    ""
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='New'/>"
    //"    <toolitem action='FileOpen'/>"
    "    <toolitem action='FileRecentFiles'/>"
    "    <toolitem action='Save'/>"
    "    <toolitem action='SaveAs'/>"
    "    <separator/>"
    "    <toolitem action='Undo'/>"
    "    <toolitem action='Redo'/>"
    "    <separator/>"
    "    <toolitem action='Cut'/>"
    "    <toolitem action='Copy'/>"
    "    <toolitem action='Paste'/>"
    "    <separator/>"
    "    <toolitem action='Duplicate'/>"
    "    <toolitem action='Delete'/>"
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
    "    <toolitem action='HFlipObject'/>"
    "    <toolitem action='VFlipObject'/>"
    "    <separator/>"
    "    <toolitem action='ConnectParent'/>"
    "    <toolitem action='ClearParent'/>"
    "    <separator/>"
    //"    <toolitem action='Snap'/>"
    "    <toolitem action='Play'/>"
    "    <separator/>"
    "  </toolbar>"
    ""
    "  <toolbar  name='ToolBox'>"
    "    <toolitem action='SelectTool'/>"
    "    <toolitem action='NavgraphInsertTool'/>"
    "    <toolitem action='ZoomTool'/>"
    "    <separator/>"
    "    <toolitem action='ToggleColorLayer'/>"
    "    <toolitem action='ToggleLightLayer'/>"
    "    <toolitem action='ToggleHighlightLayer'/>"
    "    <toolitem action='ToggleControlLayer'/>"
    "    <separator/>"
    "    <toolitem action='ToggleBackgroundLayer'/>"
    "    <toolitem action='ToggleVisibleLayer'/>"
    "    <toolitem action='ToggleGridLayer'/>"
    "  </toolbar>"
    "</ui>";

  action_group->add(Gtk::Action::create("MenuFile",    "_File"));
  action_group->add(Gtk::Action::create("New",         Gtk::Stock::NEW),
                    sigc::mem_fun(*this, &EditorWindow::on_new));
  action_group->add(Gtk::Action::create("FileOpen",    Gtk::Stock::OPEN),
                    sigc::mem_fun(*this, &EditorWindow::on_open));
  action_group->add(Gtk::Action::create("Save",        Gtk::Stock::SAVE),
                    sigc::mem_fun(*this, &EditorWindow::on_save));
  action_group->add(Gtk::Action::create("SaveAs",        Gtk::Stock::SAVE_AS),
                    sigc::mem_fun(*this, &EditorWindow::on_save_as));
  action_group->add(Gtk::Action::create("Close",       Gtk::Stock::CLOSE),
                    sigc::mem_fun(*this, &EditorWindow::on_close));
  action_group->add(Gtk::Action::create("Quit",        Gtk::Stock::QUIT),
                    sigc::mem_fun(*this, &EditorWindow::on_quit));

  action_group->add(Gtk::Action::create_with_icon_name("SaveScreenshot", "saveas", "Save Screenshot", "Save Screenshot"),
                    Gtk::AccelKey(GDK_F12, Gdk::CONTROL_MASK),
                    sigc::mem_fun(*this, &EditorWindow::on_save_screenshot));

  {
    Glib::RefPtr<Gtk::RecentAction> recent_action = Gtk::RecentAction::create("FileRecentFiles", Gtk::Stock::OPEN);

    // Not sure what this does, but it seems to help bring recent items up to the top
    recent_action->set_sort_type(Gtk::RECENT_SORT_MRU);

    // Show only file:// resources
    recent_action->set_local_only();

    recent_action->set_limit(25);

    Gtk::RecentFilter* filter= Gtk::manage(new Gtk::RecentFilter);
    filter->add_mime_type("application/windstille");
    //filter->add_application("Windstille Editor");
    //filter->add_pattern("*.wst");
    recent_action->set_filter(*filter);

    recent_action->signal_item_activated().connect(sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_recent_file), recent_action));
    action_group->add(recent_action, 
                      sigc::mem_fun(*this, &EditorWindow::on_open));
  }

  action_group->add(Gtk::Action::create("MenuEdit",    "_Edit"));
  action_group->add(Gtk::Action::create("Undo",        Gtk::Stock::UNDO),
                    sigc::mem_fun(*this, &EditorWindow::on_undo));
  action_group->add(Gtk::Action::create("Redo",        Gtk::Stock::REDO),
                    sigc::mem_fun(*this, &EditorWindow::on_redo));
  action_group->add(Gtk::Action::create("Cut",         Gtk::Stock::CUT),
                    sigc::mem_fun(*this, &EditorWindow::on_cut));
  action_group->add(Gtk::Action::create("Copy",        Gtk::Stock::COPY),
                    sigc::mem_fun(*this, &EditorWindow::on_copy));
  action_group->add(Gtk::Action::create("Paste",       Gtk::Stock::PASTE),
                    sigc::mem_fun(*this, &EditorWindow::on_paste));
  action_group->add(Gtk::Action::create("SelectAll",       Gtk::Stock::SELECT_ALL),
                    Gtk::AccelKey("<control>a"),
                    sigc::mem_fun(*this, &EditorWindow::on_select_all));

  action_group->add(Gtk::Action::create("Delete",      Gtk::Stock::DELETE),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_delete));
  action_group->add(Gtk::Action::create_with_icon_name("Duplicate", "duplicate", "Duplicate Object", "Duplicate Object"),
                    Gtk::AccelKey(GDK_d, Gdk::CONTROL_MASK),
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

  action_group->add(Gtk::Action::create_with_icon_name("ResetRotation", "reload", "Reset Rotation", "Reset Rotation"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_reset_rotation));
  action_group->add(Gtk::Action::create_with_icon_name("ResetScale", "reload", "Reset Scale", "Reset Scale"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_reset_scale));

  action_group->add(Gtk::Action::create_with_icon_name("ObjectProperties", "properties", "Object Properties", "Object Properties"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_object_properties));

  action_group->add(Gtk::Action::create_with_icon_name("HFlipObject", "object_hflip", "Horizontal Flip", "Horizontal Flip"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_hflip));
  action_group->add(Gtk::Action::create_with_icon_name("VFlipObject", "object_vflip", "Vertical Flip", "Vertical Flip"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_windstille_widget), &WindstilleWidget::selection_vflip));

  action_group->add(Gtk::Action::create("MenuView",    "_View"));
  action_group->add(Gtk::Action::create("Zoom100",     Gtk::Stock::ZOOM_100),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_100));
  action_group->add(Gtk::Action::create("ZoomIn",      Gtk::Stock::ZOOM_IN),
                    Gtk::AccelKey(GDK_plus, Gdk::CONTROL_MASK),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_in));
  action_group->add(Gtk::Action::create("ZoomOut",     Gtk::Stock::ZOOM_OUT),
                    Gtk::AccelKey(GDK_minus, Gdk::CONTROL_MASK),
                    sigc::mem_fun(*this, &EditorWindow::on_zoom_out));
  action_group->add(play_action = Gtk::ToggleAction::create("Play", Gtk::Stock::MEDIA_PLAY), 
                    sigc::mem_fun(*this, &EditorWindow::on_play));

  action_group->add(Gtk::Action::create("MenuHelp",    "_Help"));
  action_group->add(Gtk::Action::create("About",       Gtk::Stock::ABOUT),
                    sigc::mem_fun(*this, &EditorWindow::on_about_clicked));

  toggle_color_layer     = Gtk::ToggleAction::create_with_icon_name("ToggleColorLayer", "color", "Toogle Color Layer", "Toogle Color Layer");
  toggle_light_layer     = Gtk::ToggleAction::create_with_icon_name("ToggleLightLayer", "light", "Toogle Light Layer", "Toogle Light Layer");
  toggle_highlight_layer = Gtk::ToggleAction::create_with_icon_name("ToggleHighlightLayer", "highlight", "Toogle Highlight Layer", "Toogle Highlight Layer");
  toggle_control_layer   = Gtk::ToggleAction::create_with_icon_name("ToggleControlLayer", "control", "Toogle Control Layer", "Toogle Control Layer");

  background_layer = Gtk::ToggleAction::create_with_icon_name("ToggleBackgroundLayer", "background_layer", "Toggle Background Layer", "Toggle Background Layer");
  visible_layer    = Gtk::ToggleAction::create_with_icon_name("ToggleVisibleLayer", "draw_visible_layer", "Toggle Only Active Layer", "Toggle Only Active Layer");
  grid_layer       = Gtk::ToggleAction::create_with_icon_name("ToggleGridLayer", "grid", "Toggle Grid Layer", "Toggle Grid Layer");
  
  toggle_color_layer->set_active(true);
  toggle_light_layer->set_active(false);
  toggle_highlight_layer->set_active(true);
  toggle_control_layer->set_active(true);
  background_layer->set_active(true);
  visible_layer->set_active(true);
  grid_layer->set_active(false);

  action_group->add(toggle_color_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_color_layer, (unsigned int)SceneContext::COLORMAP));
  action_group->add(toggle_light_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_light_layer, (unsigned int)SceneContext::LIGHTMAP));
  action_group->add(toggle_highlight_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_highlight_layer, (unsigned int)SceneContext::HIGHLIGHTMAP));
  action_group->add(toggle_control_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), toggle_control_layer, (unsigned int)SceneContext::CONTROLMAP));
  action_group->add(background_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_background_layer), background_layer));
  action_group->add(visible_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_draw_only_active_layer), visible_layer));
  action_group->add(grid_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_grid_layer), grid_layer));

  // Tools
  action_group->add(Gtk::Action::create("MenuTools",  "_Tools"));
  Gtk::RadioButtonGroup tool_group;
  
  select_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "SelectTool", "select_tool", "Select Tool", "Select Tool");
  navgraph_insert_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "NavgraphInsertTool", "navgraph_insert_tool",   "Navgraph Insert Tool", "Navgraph Insert Tool");
  zoom_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "ZoomTool", "zoom_tool",   "Zoom Tool", "Zoom Tool");

  action_group->add(select_tool_action, sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), select_tool_action, select_tool.get()));
  action_group->add(navgraph_insert_tool_action, sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), navgraph_insert_tool_action, navgraph_insert_tool.get()));
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

  Gtk::Toolbar* toolbar = static_cast<Gtk::Toolbar*>(ui_manager->get_widget("/ToolBar"));
  toolbar->append(*(Gtk::manage(new Gtk::SeparatorToolItem())));
  layer_widget = Gtk::manage(new LayerWidget());
  toolbar->append(*layer_widget);
  layer_widget->signal_layer_toggle.connect(sigc::mem_fun(*this, &EditorWindow::on_layer_toggle));

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
  //layer_manager.set_size_request(250, 300);
  
  hpaned.pack1(notebook,     Gtk::EXPAND);
  hpaned.pack2(sidebar_vbox, Gtk::SHRINK);
 
  sidebar_vbox.pack_start(vpaned, Gtk::PACK_EXPAND_WIDGET);
  sidebar_vbox.pack_start(minimap_widget, Gtk::PACK_SHRINK);
  
  vpaned.pack1(object_selector, Gtk::EXPAND);
  vpaned.pack2(layer_manager,     Gtk::SHRINK);

  hpaned.set_position(970);
  vpaned.set_position(420);

  // Window
  add(vbox);

  object_selector.populate();
}

EditorWindow::~EditorWindow()
{
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
  //std::cout << "on_new" << std::endl;

  // FIXME: We abuse the minimap as our root GLContext
  WindstilleWidget* wst = Gtk::manage(new WindstilleWidget(*this, glconfig, minimap_widget.get_gl_context()));

  Glib::ustring title = Glib::ustring::compose("Unsaved Sector %1", notebook.get_n_pages()+1);
  int new_page = notebook.append_page(*wst, title);
  wst->show();
  notebook.set_current_page(new_page);

  layer_manager.set_model(wst->get_sector_model());
  layer_widget->update(wst->get_select_mask());
}

void
EditorWindow::load_file(const std::string& filename)
{
  try 
    {
      on_new();
      WindstilleWidget* wst = get_windstille_widget();
      wst->load_file(filename);
      wst->set_filename(filename);
      notebook.set_tab_label_text(*notebook.get_nth_page(notebook.get_current_page()), Glib::path_get_basename(filename));

      print("Loaded: " + filename);
    }
  catch(std::exception& err)
    {
      std::ostringstream str;
      str << "EditorWindow::load_file: " << err.what();
      print(str.str());
    }
}

void
EditorWindow::on_open()
{
  Gtk::FileChooserDialog dialog("Open File",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN,   Gtk::RESPONSE_OK);
  dialog.set_current_folder("data/sectors/");

  switch(dialog.run())
    {
      case(Gtk::RESPONSE_OK):
        {
          //std::cout << "Select clicked." << std::endl;
          //std::cout << "Folder selected: " << dialog.get_filename()
          //          << std::endl;
          
          add_recent_file(dialog.get_filename());

          load_file(dialog.get_filename());
          break;
        }

      case(Gtk::RESPONSE_CANCEL):
        {
          //std::cout << "Cancel clicked." << std::endl;
          break;
        }
    }
}

void
EditorWindow::on_save()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      if (wst->get_filename().empty())
        {
          on_save_as();
        }
      else
        {
          std::ofstream out(wst->get_filename().c_str());
          FileWriter writer(out);
          wst->get_sector_model()->write(writer);
          print("Wrote: " + wst->get_filename());
        }
    }
}

void
EditorWindow::on_save_as()
{
  int page = notebook.get_current_page();
  if (page == -1)
    {
      // do nothing;
    }
  else
    {
      Gtk::FileChooserDialog dialog("Save File",
                                    Gtk::FILE_CHOOSER_ACTION_SAVE);
      dialog.set_transient_for(*this);
      dialog.set_do_overwrite_confirmation(true);

      dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      dialog.add_button(Gtk::Stock::SAVE,   Gtk::RESPONSE_OK);

      WindstilleWidget* wst = static_cast<WindstilleWidget*>(notebook.get_nth_page(page));
 
      if (wst->get_filename().empty())
        dialog.set_current_folder("data/sectors/");
      else
        dialog.set_current_folder(Glib::path_get_dirname(wst->get_filename()));

      switch(dialog.run())
        {
          case(Gtk::RESPONSE_OK):
            {
              //std::cout << "Select clicked." << std::endl;
              //std::cout << "Folder selected: " << dialog.get_filename()
              //          << std::endl;

              std::string filename = dialog.get_filename();
              std::ofstream out(filename.c_str());
              FileWriter writer(out);

              wst->get_sector_model()->write(writer);
              wst->set_filename(filename);

              notebook.set_tab_label_text(*notebook.get_nth_page(page), Glib::path_get_basename(filename));
              add_recent_file(filename);
              print("Wrote: " + filename);
              break;
            }

          case(Gtk::RESPONSE_CANCEL):
            {
              //std::cout << "Cancel clicked." << std::endl;
              break;
            }
        }
    }
}

void
EditorWindow::on_save_screenshot()
{
  int page = notebook.get_current_page();
  if (page == -1)
    {
      // do nothing;
    }
  else
    {
      if (WindstilleWidget* wst = static_cast<WindstilleWidget*>(notebook.get_nth_page(page)))
        {
          Gtk::FileChooserDialog dialog("Save Screenshot",
                                        Gtk::FILE_CHOOSER_ACTION_SAVE);
          dialog.set_transient_for(*this);
          dialog.set_do_overwrite_confirmation(true);

          dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
          dialog.add_button(Gtk::Stock::SAVE,   Gtk::RESPONSE_OK);

 
          dialog.set_current_folder("/tmp/");

          switch(dialog.run())
            {
              case(Gtk::RESPONSE_OK):
                {
                  std::string filename = dialog.get_filename();
                  std::cout << "unimplemented screenshot: " << filename << std::endl;
                  wst->save_screenshot(filename);
                  break;
                }

              case(Gtk::RESPONSE_CANCEL):
                {
                  //std::cout << "Cancel clicked." << std::endl;
                  break;
                }
            }
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
        layer_manager.set_model(0);
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
EditorWindow::update_undo_state()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      action_group->get_action("Undo")->set_sensitive(wst->get_undo_manager()->can_undo());
      action_group->get_action("Redo")->set_sensitive(wst->get_undo_manager()->can_redo());
    }
}

void
EditorWindow::on_undo()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->undo();
      update_undo_state();
      queue_draw();
    }
}

void
EditorWindow::on_redo()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->redo();
      update_undo_state();
      queue_draw();
    }
}

void
EditorWindow::on_tool_select(Glib::RefPtr<Gtk::RadioAction> action, Tool* tool)
{
  //std::cout << "on_tool_select()" << action->get_active() << std::endl;
  if (action->get_active())
    {
      current_tool = tool;
      if (WindstilleWidget* wst = get_windstille_widget())
        wst->queue_draw();
    }
}

void
EditorWindow::toggle_render_layer(Glib::RefPtr<Gtk::ToggleAction> action, unsigned int mask)
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

      //std::cout << "mask: " << sc.get_render_mask() << std::endl;
      wst->queue_draw();
    }
}

void
EditorWindow::toggle_draw_only_active_layer(Glib::RefPtr<Gtk::ToggleAction> action)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->set_draw_only_active_layer(action->get_active());
      wst->queue_draw();
    }
}

void
EditorWindow::toggle_background_layer(Glib::RefPtr<Gtk::ToggleAction> action)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->set_draw_background_pattern(action->get_active());
      wst->queue_draw();
    }
}

void
EditorWindow::toggle_grid_layer(Glib::RefPtr<Gtk::ToggleAction> action)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->enable_grid(action->get_active());
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

bool
EditorWindow::get_draw_navgraph() const
{
  if (current_tool == navgraph_insert_tool.get())
    return true;
  else
    return false;
}

void
EditorWindow::on_switch_page(GtkNotebookPage* /*page*/, guint /*page_num*/)
{
  //std::cout << "on_switch_page(" << page << ", " << page_num << ")" << std::endl;

  if (WindstilleWidget* wst = get_windstille_widget())
    {
      layer_manager.set_model(wst->get_sector_model());
      layer_widget->update(wst->get_select_mask());

      toggle_color_layer->set_active(wst->get_sc()->get_render_mask() & SceneContext::COLORMAP);
      toggle_light_layer->set_active(wst->get_sc()->get_render_mask() & SceneContext::LIGHTMAP);
      toggle_highlight_layer->set_active(wst->get_sc()->get_render_mask() & SceneContext::HIGHLIGHTMAP);
      toggle_control_layer->set_active(wst->get_sc()->get_render_mask() & SceneContext::CONTROLMAP);

      background_layer->set_active(wst->get_draw_background_pattern());
      visible_layer->set_active(wst->get_draw_only_active_layer());
      grid_layer->set_active(wst->get_enable_grid());

      update_undo_state();
    }
  else
    {
      layer_manager.set_model(0);
    }
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
EditorWindow::on_layer_toggle(int layer, bool status_)
{
  //std::cout << "EditorWindow::on_layer_toggle(" << layer << ", " << status << ")" << std::endl;

  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->get_select_mask().set(layer, status_);
      wst->queue_draw();
    }
}

void
EditorWindow::on_play()
{
  if (play_action->get_active())
    {
      //std::cout << "Play" << std::endl;
      timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &EditorWindow::on_timeout),
                                                          50,
                                                          Glib::PRIORITY_DEFAULT);
    }
  else
    {
      //std::cout << "Stop" << std::endl;
      timeout_connection.disconnect();
    }
}

void
EditorWindow::on_cut()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      clipboard = wst->get_selection()->clone();
      wst->selection_delete();
      queue_draw();
    }
}

void
EditorWindow::on_copy()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      clipboard = wst->get_selection()->clone();
    }
}

void
EditorWindow::on_delete_layer()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      //std::cout << "Deleting layer: " << wst << std::endl;
      //wst->get_sector_model()->delete_layer(wst->get_current_layer_path());

      CommandHandle cmd(new LayerDeleteCommand(*wst->get_sector_model(), wst->get_current_layer_path()));
      wst->execute(cmd);
      queue_draw();
    }
}

void
EditorWindow::on_reverse_layers()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->get_sector_model()->reverse_layers();
    }
}

void
EditorWindow::on_new_layer()
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      //std::cout << "Adding layer" << std::endl;
      //wst->get_sector_model()->add_layer("New Layer", wst->get_current_layer_path());

      CommandHandle cmd(new LayerAddCommand(*wst->get_sector_model(), wst->get_current_layer_path()));
      wst->execute(cmd);

      layer_manager.get_treeview().expand_all();
    }
}

void
EditorWindow::on_paste()
{
  if (clipboard.get())
    {
      if (WindstilleWidget* wst = get_windstille_widget())
        {
          LayerHandle layer = wst->get_current_layer();
          for(Selection::reverse_iterator i = clipboard->rbegin(); i != clipboard->rend(); ++i)
            {
              layer->add(*i);
            }

          wst->set_selection(clipboard);
          clipboard = clipboard->clone();
          queue_draw();
        }
    }
}

void
EditorWindow::on_select_all()
{
  // Select all on current layer
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      LayerHandle layer = wst->get_current_layer();
      SelectionHandle selection = Selection::create();
      selection->add(layer->begin(), layer->end());
      wst->set_selection(selection);
    }
}

void
EditorWindow::on_show_all(bool v)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->get_sector_model()->set_all_visible(v);
      wst->queue_draw();
    }
}

void
EditorWindow::on_lock_all(bool v)
{
  if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->get_sector_model()->set_all_locked(v);
      wst->queue_draw();
    }
}

void
EditorWindow::on_recent_file(const Glib::RefPtr<Gtk::RecentAction>& recent_action)
{
  Glib::RefPtr<const Gtk::RecentInfo> item = recent_action->get_current_item();

  //std::cout << "On Recent File:" << item->get_uri() << std::endl;
  if (item->exists())
    load_file(Glib::filename_from_uri(item->get_uri()));
}

void
EditorWindow::add_recent_file(const std::string& filename)
{
  Gtk::RecentManager::Data data;
  data.display_name = Glib::path_get_basename(filename);
  data.app_name     = "Windstille Editor";
  data.app_exec     = "windstille-editor";
  data.groups.push_back("Windstille");
  data.is_private   = false;
  data.mime_type    = "application/windstille";

  Gtk::RecentManager::get_default()->add_item(Glib::filename_to_uri(filename), data);
}

bool
EditorWindow::remove_message(guint id)
{
  status.remove_message(id);
  return false;
}

void
EditorWindow::print(const std::string& text)
{
  guint id = status.push(text);
  std::cout << "[LOG] " << text << std::endl;
  Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &EditorWindow::remove_message), id), 6000);
}

/* EOF */
