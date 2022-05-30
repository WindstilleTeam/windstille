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

#include "editor/editor_window.hpp"

#include <fstream>
#include <iostream>

#include <gdkmm/pixbuf.h>
#include <glibmm/convert.h>
#include <glibmm/main.h>
#include <glibmm/miscutils.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/menubar.h>
#include <gtkmm/paned.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/recentaction.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>

#include <logmich/log.hpp>

#include <wstdisplay/scene_context.hpp>
#include "editor/windstille_widget.hpp"
#include "editor/about_window.hpp"
#include "editor/editor_window.hpp"
#include "editor/zoom_tool.hpp"
#include "editor/scroll_tool.hpp"
#include "editor/select_tool.hpp"
#include "editor/navgraph_insert_tool.hpp"
#include "editor/sector_model.hpp"
#include "editor/layer_widget.hpp"
#include "editor/document.hpp"

#include "editor/animation_widget.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_keyframe_object.hpp"
#include "editor/timeline_layer.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_widget.hpp"

EditorWindow::EditorWindow() :
  m_vbox(),
  m_sidebar_vbox(),
  m_hbox(),
  m_hpaned(),
  m_vpaned(),
  m_status_hbox(),
  m_status_label(),
  m_statusbar(),
  m_ui_manager(Gtk::UIManager::create()),
  m_action_group(Gtk::ActionGroup::create("EditorWindow")),
  m_share_list(),
  m_notebook(),
  m_object_selector(*this),
  m_layer_manager(*this),
  m_minimap_widget(),
  m_select_tool_action(),
  m_navgraph_insert_tool_action(),
  m_navgraph_select_tool_action(),
  m_zoom_tool_action(),
  m_toggle_color_layer(),
  m_toggle_light_layer(),
  m_toggle_highlight_layer(),
  m_toggle_control_layer(),
  m_background_layer(),
  m_visible_layer(),
  m_grid_layer(),
  m_play_action(),
  m_snap_action(),
  m_select_tool(new SelectTool(*this)),
  m_navgraph_insert_tool(new NavgraphInsertTool()),
  m_zoom_tool(new ZoomTool()),
  m_scroll_tool(new ScrollTool()),
  m_current_tool(m_select_tool.get()),
  m_layer_widget(),
  m_timeout_connection(),
  m_clipboard()
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
    "      <menuitem action='ToggleRGBAfLayer'/>"
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
    "  <toolbar name='ToolBar'>"
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
    "    <toolitem action='ToggleRGBAfLayer'/>"
    "    <toolitem action='ToggleLightLayer'/>"
    "    <toolitem action='ToggleHighlightLayer'/>"
    "    <toolitem action='ToggleControlLayer'/>"
    "    <separator/>"
    "    <toolitem action='ToggleBackgroundLayer'/>"
    "    <toolitem action='ToggleVisibleLayer'/>"
    "    <toolitem action='ToggleGridLayer'/>"
    "  </toolbar>"
    "</ui>";

  m_action_group->add(Gtk::Action::create("MenuFile",    "_File"));
  m_action_group->add(Gtk::Action::create("New",         Gtk::Stock::NEW),
                    [this](){ on_new(); });
  m_action_group->add(Gtk::Action::create("FileOpen",    Gtk::Stock::OPEN),
                    [this](){ on_open(); });
  m_action_group->add(Gtk::Action::create("Save",        Gtk::Stock::SAVE),
                    [this](){ on_save(); });
  m_action_group->add(Gtk::Action::create("SaveAs",        Gtk::Stock::SAVE_AS),
                    [this](){ on_save_as(); });
  m_action_group->add(Gtk::Action::create("Close",       Gtk::Stock::CLOSE),
                    [this](){ on_close(); });
  m_action_group->add(Gtk::Action::create("Quit",        Gtk::Stock::QUIT),
                    [this](){ on_quit(); });

  m_action_group->add(Gtk::Action::create_with_icon_name("SaveScreenshot", "saveas", "Save Screenshot", "Save Screenshot"),
                    Gtk::AccelKey(GDK_KEY_F12, Gdk::CONTROL_MASK),
                    [this](){ on_save_screenshot(); });

  {
    Glib::RefPtr<Gtk::RecentAction> recent_action = Gtk::RecentAction::create("FileRecentFiles", Gtk::Stock::OPEN);

    // Not sure what this does, but it seems to help bring recent items up to the top
    recent_action->set_sort_type(Gtk::RECENT_SORT_MRU);

    // Show only file:// resources
    recent_action->set_local_only();

    recent_action->set_limit(25);

    Glib::RefPtr<Gtk::RecentFilter> filter = Gtk::RecentFilter::create();
    filter->add_mime_type("application/windstille");
    //filter->add_application("Windstille Editor");
    //filter->add_pattern("*.wst");
    recent_action->set_filter(filter);

    recent_action->signal_item_activated().connect(sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_recent_file), recent_action));
    m_action_group->add(recent_action,
                      [this](){ on_open(); });
  }

  m_action_group->add(Gtk::Action::create("MenuEdit",    "_Edit"));
  m_action_group->add(Gtk::Action::create("Undo",        Gtk::Stock::UNDO),
                    [this](){ on_undo(); });
  m_action_group->add(Gtk::Action::create("Redo",        Gtk::Stock::REDO),
                    [this](){ on_redo(); });
  m_action_group->add(Gtk::Action::create("Cut",         Gtk::Stock::CUT),
                    [this](){ on_cut(); });
  m_action_group->add(Gtk::Action::create("Copy",        Gtk::Stock::COPY),
                    [this](){ on_copy(); });
  m_action_group->add(Gtk::Action::create("Paste",       Gtk::Stock::PASTE),
                    [this](){ on_paste(); });
  m_action_group->add(Gtk::Action::create("SelectAll",       Gtk::Stock::SELECT_ALL),
                    Gtk::AccelKey("<control>a"),
                    [this](){ on_select_all(); });

  m_action_group->add(Gtk::Action::create("Delete",      Gtk::Stock::DELETE),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_delete));
  m_action_group->add(Gtk::Action::create_with_icon_name("Duplicate", "duplicate", "Duplicate Object", "Duplicate Object"),
                    Gtk::AccelKey(GDK_KEY_d, Gdk::CONTROL_MASK),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_duplicate));

  m_action_group->add(Gtk::Action::create("MenuObject",    "_Object"));
  m_action_group->add(Gtk::Action::create_with_icon_name("RaiseObjectToTop", "object_raise_to_top", "Raise To Top", "Raise Object to Top"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_raise_to_top));
  m_action_group->add(Gtk::Action::create_with_icon_name("RaiseObject", "object_raise", "Raise", "Raise Object"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_raise));
  m_action_group->add(Gtk::Action::create_with_icon_name("LowerObject", "object_lower", "Lower", "Lower Object"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_lower));
  m_action_group->add(Gtk::Action::create_with_icon_name("LowerObjectToBottom", "object_lower_to_bottom", "Lower To Bottom", "Lower Object to Bottom"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_lower_to_bottom));

  m_action_group->add(Gtk::Action::create_with_icon_name("ConnectParent", "connect_parent", "Connect Parent", "Connect Parent"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_connect_parent));
  m_action_group->add(Gtk::Action::create_with_icon_name("ClearParent", "clear_parent", "Clear Parent", "Clear Parent"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_clear_parent));

  m_action_group->add(Gtk::Action::create_with_icon_name("ResetRotation", "reload", "Reset Rotation", "Reset Rotation"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_reset_rotation));
  m_action_group->add(Gtk::Action::create_with_icon_name("ResetScale", "reload", "Reset Scale", "Reset Scale"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_reset_scale));

  m_action_group->add(Gtk::Action::create_with_icon_name("ObjectProperties", "properties", "Object Properties", "Object Properties"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_object_properties));

  m_action_group->add(Gtk::Action::create_with_icon_name("HFlipObject", "object_hflip", "Horizontal Flip", "Horizontal Flip"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_hflip));
  m_action_group->add(Gtk::Action::create_with_icon_name("VFlipObject", "object_vflip", "Vertical Flip", "Vertical Flip"),
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::call_with_document), &Document::selection_vflip));

  m_action_group->add(Gtk::Action::create("MenuView",    "_View"));
  m_action_group->add(Gtk::Action::create("Zoom100",     Gtk::Stock::ZOOM_100),
                    [this](){ on_zoom_100(); });
  m_action_group->add(Gtk::Action::create("ZoomIn",      Gtk::Stock::ZOOM_IN),
                    Gtk::AccelKey(GDK_KEY_plus, Gdk::CONTROL_MASK),
                    [this](){ on_zoom_in(); });
  m_action_group->add(Gtk::Action::create("ZoomOut",     Gtk::Stock::ZOOM_OUT),
                    Gtk::AccelKey(GDK_KEY_minus, Gdk::CONTROL_MASK),
                    [this](){ on_zoom_out(); });
  m_action_group->add(m_play_action = Gtk::ToggleAction::create("Play", Gtk::Stock::MEDIA_PLAY),
                    [this](){ on_play(); });

  m_action_group->add(Gtk::Action::create("MenuHelp",    "_Help"));
  m_action_group->add(Gtk::Action::create("About",       Gtk::Stock::ABOUT),
                    [this](){ on_about_clicked(); });

  m_toggle_color_layer     = Gtk::ToggleAction::create_with_icon_name("ToggleRGBAfLayer", "color", "Toggle RGBAf Layer", "Toggle RGBAf Layer");
  m_toggle_light_layer     = Gtk::ToggleAction::create_with_icon_name("ToggleLightLayer", "light", "Tooggle Light Layer", "Toggle Light Layer");
  m_toggle_highlight_layer = Gtk::ToggleAction::create_with_icon_name("ToggleHighlightLayer", "highlight", "Toggle Highlight Layer", "Toogle Highlight Layer");
  m_toggle_control_layer   = Gtk::ToggleAction::create_with_icon_name("ToggleControlLayer", "control", "Toggle Control Layer", "Toogle Control Layer");

  m_background_layer = Gtk::ToggleAction::create_with_icon_name("ToggleBackgroundLayer", "background_layer", "Toggle Background Layer", "Toggle Background Layer");
  m_visible_layer    = Gtk::ToggleAction::create_with_icon_name("ToggleVisibleLayer", "draw_visible_layer", "Toggle Only Active Layer", "Toggle Only Active Layer");
  m_grid_layer       = Gtk::ToggleAction::create_with_icon_name("ToggleGridLayer", "grid", "Toggle Grid Layer", "Toggle Grid Layer");

  m_toggle_color_layer->set_active(true);
  m_toggle_light_layer->set_active(false);
  m_toggle_highlight_layer->set_active(true);
  m_toggle_control_layer->set_active(true);
  m_background_layer->set_active(true);
  m_visible_layer->set_active(true);
  m_grid_layer->set_active(false);

  m_action_group->add(m_toggle_color_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), m_toggle_color_layer, static_cast<unsigned int>(wstdisplay::SceneContext::COLORMAP)));
  m_action_group->add(m_toggle_light_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), m_toggle_light_layer, static_cast<unsigned int>(wstdisplay::SceneContext::LIGHTMAP)));
  m_action_group->add(m_toggle_highlight_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), m_toggle_highlight_layer, static_cast<unsigned int>(wstdisplay::SceneContext::HIGHLIGHTMAP)));
  m_action_group->add(m_toggle_control_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_render_layer), m_toggle_control_layer, static_cast<unsigned int>(wstdisplay::SceneContext::CONTROLMAP)));
  m_action_group->add(m_background_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_background_layer), m_background_layer));
  m_action_group->add(m_visible_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_draw_only_active_layer), m_visible_layer));
  m_action_group->add(m_grid_layer,
                    sigc::bind(sigc::mem_fun(*this, &EditorWindow::toggle_grid_layer), m_grid_layer));

  // Tools
  m_action_group->add(Gtk::Action::create("MenuTools",  "_Tools"));
  Gtk::RadioButtonGroup tool_group;

  m_select_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "SelectTool", "select_tool", "Select Tool", "Select Tool");
  m_navgraph_insert_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "NavgraphInsertTool", "navgraph_insert_tool",   "Navgraph Insert Tool", "Navgraph Insert Tool");
  m_zoom_tool_action = Gtk::RadioAction::create_with_icon_name(tool_group, "ZoomTool", "zoom_tool",   "Zoom Tool", "Zoom Tool");

  m_action_group->add(m_select_tool_action, sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), m_select_tool_action, m_select_tool.get()));
  m_action_group->add(m_navgraph_insert_tool_action, sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), m_navgraph_insert_tool_action, m_navgraph_insert_tool.get()));
  m_action_group->add(m_zoom_tool_action,   sigc::bind(sigc::mem_fun(*this, &EditorWindow::on_tool_select), m_zoom_tool_action, m_zoom_tool.get()));

  // signal_size_allocate().connect (sigc::mem_fun (*this, &EditorWindow::on_window_size_allocate), false);
  // signal_realize().connect (sigc::mem_fun (*this, &EditorWindow::on_window_realize));

  m_ui_manager->insert_action_group(m_action_group);

  m_ui_manager->add_ui_from_string(ui_info);

  add_accel_group(m_ui_manager->get_accel_group());

  m_notebook.signal_switch_page().connect([this](Gtk::Widget* widget, guint page_num){
    on_switch_page(widget, page_num);
  });

  // Disable unimplemented stuff:
  m_action_group->get_action("Undo")->set_sensitive(false);
  m_action_group->get_action("Redo")->set_sensitive(false);

  Gtk::MenuBar& menubar = dynamic_cast<Gtk::MenuBar&>(*m_ui_manager->get_widget("/MenuBar"));
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*m_ui_manager->get_widget("/ToolBar"));
  Gtk::Toolbar& toolbox = dynamic_cast<Gtk::Toolbar&>(*m_ui_manager->get_widget("/ToolBox"));

  toolbox.get_style_context()->add_class("toolbox");

  toolbar.append(*(Gtk::make_managed<Gtk::SeparatorToolItem>()));
  m_layer_widget = Gtk::make_managed<LayerWidget>();
  toolbar.append(*m_layer_widget);

  m_layer_widget->signal_layer_toggle.connect([this](int layer, bool status){ on_layer_toggle(layer, status); });

  toolbox.set_property("orientation", Gtk::ORIENTATION_VERTICAL);
  toolbox.set_property("toolbar-style", Gtk::TOOLBAR_ICONS);

  // Packing

  // Main Vbox
  m_vbox.pack_start(menubar, Gtk::PACK_SHRINK);
  m_vbox.pack_start(toolbar, Gtk::PACK_SHRINK);
  m_vbox.add(m_hbox);

  m_status_hbox.pack_start(m_status_label, Gtk::PACK_SHRINK);
  m_status_hbox.pack_start(m_statusbar, Gtk::PACK_EXPAND_WIDGET);
  m_vbox.pack_end(m_status_hbox, Gtk::PACK_SHRINK);

  // Hbox
  m_hbox.pack_start(toolbox, Gtk::PACK_SHRINK);
  m_hbox.add(m_hpaned);

  // vpaned.set_size_request(250, -1);
  //m_object_selector.set_size_request(250, 300);
  //m_layer_manager.set_size_request(250, 300);

  m_hpaned.pack1(m_notebook,     Gtk::EXPAND);
  m_hpaned.pack2(m_sidebar_vbox);

  m_vpaned.pack1(m_object_selector, Gtk::EXPAND);
  m_vpaned.pack2(m_layer_manager, Gtk::SHRINK);

  m_sidebar_vbox.pack_start(m_vpaned, Gtk::PACK_EXPAND_WIDGET);
  //m_sidebar_vbox.pack_start(m_minimap_widget, Gtk::PACK_SHRINK);

  m_hpaned.set_position(970);
  m_vpaned.set_position(420);

  // Window
  add(m_vbox);
  m_object_selector.populate();
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
  // FIXME: We abuse the minimap as our root GLContext
  Gtk::VPaned* paned = Gtk::make_managed<Gtk::VPaned>();
  WindstilleWidget* wst = Gtk::make_managed<WindstilleWidget>(*this);
  AnimationWidget* animation_widget = Gtk::make_managed<AnimationWidget>(*this);

  paned->pack1(*wst, Gtk::FILL|Gtk::EXPAND);
  paned->pack2(*animation_widget, Gtk::FILL|Gtk::EXPAND);
  paned->set_position(600);

  { // FIXME: some random data for testing
    std::shared_ptr<Timeline> timeline = wst->get_document().get_sector_model().get_timeline();

    TimelineLayerHandle layer1 = timeline->add_layer("Layer1");

    layer1->add_object(TimelineObjectHandle(new TimelineAnimObject(10.0f, 15.0f, "anim1.anim")));
    layer1->add_object(TimelineObjectHandle(new TimelineAnimObject(60.0f, 12.0f, "anim2.anim")));
    layer1->add_object(TimelineObjectHandle(new TimelineSoundObject(30.0f, 15.0f, "sound.wav")));

    animation_widget->set_timeline(timeline);
  }

  Glib::ustring title = Glib::ustring::compose("Unsaved Sector %1", m_notebook.get_n_pages()+1);
  int new_page = m_notebook.append_page(*paned, title);
  paned->show_all();
  m_notebook.set_current_page(new_page);

  m_layer_manager.set_model(&wst->get_document().get_sector_model());
  m_layer_widget->update(wst->get_select_mask());
}

void
EditorWindow::load_file(std::string const& filename)
{
  try
  {
    on_new();
    WindstilleWidget* wst = get_windstille_widget();
    AnimationWidget*  animation_widget = get_animation_widget();

    wst->load_file(filename);
    wst->set_filename(filename);
    m_notebook.set_tab_label_text(*m_notebook.get_nth_page(m_notebook.get_current_page()), Glib::path_get_basename(filename));
    m_layer_manager.set_model(&wst->get_document().get_sector_model());
    animation_widget->set_timeline(wst->get_document().get_sector_model().get_timeline());
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
  dialog.set_current_folder(Pathname("sectors/", Pathname::kDataPath).get_sys_path());

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
      wst->get_document().get_sector_model().write(writer);
      print("Wrote: " + wst->get_filename());
    }
  }
}

void
EditorWindow::on_save_as()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    Gtk::FileChooserDialog dialog("Save File",
                                  Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);
    dialog.set_do_overwrite_confirmation(true);

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,   Gtk::RESPONSE_OK);

    if (wst->get_filename().empty())
      dialog.set_current_folder(Pathname("sectors", Pathname::kDataPath).get_sys_path());
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

        wst->get_document().get_sector_model().write(writer);
        wst->set_filename(filename);

        int page = m_notebook.get_current_page();
        m_notebook.set_tab_label_text(*m_notebook.get_nth_page(page), Glib::path_get_basename(filename));
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
  int page = m_notebook.get_current_page();
  if (page == -1)
  {
    // do nothing;
  }
  else
  {
    if (WindstilleWidget* wst = static_cast<WindstilleWidget*>(m_notebook.get_nth_page(page)))
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
  int page = m_notebook.get_current_page();
  if (page != -1)
  {
    m_notebook.remove_page(page);

    if (!get_windstille_widget())
      m_layer_manager.set_model(nullptr);
  }
}

void
EditorWindow::show_minimap(bool v)
{
  if (v)
  {
    m_minimap_widget.show();
  }
  else
  {
    m_minimap_widget.hide();
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
    m_action_group->get_action("Undo")->set_sensitive(wst->get_document().has_undo());
    m_action_group->get_action("Redo")->set_sensitive(wst->get_document().has_redo());
  }
}

void
EditorWindow::on_undo()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().undo();
    update_undo_state();
    queue_draw();
  }
}

void
EditorWindow::on_redo()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().redo();
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
    m_current_tool = tool;
    if (WindstilleWidget* wst = get_windstille_widget())
      wst->queue_draw();
  }
}

void
EditorWindow::on_animation_new()
{
}

void
EditorWindow::on_animation_export()
{
}
void
EditorWindow::on_animation_delete()
{
}

void
EditorWindow::on_animation_frame_forward()
{
}

void
EditorWindow::on_animation_frame_backward()
{
}

void
EditorWindow::on_animation_layer_new()
{
  if (Document* document = get_document())
  {
    document->timeline_add_layer("AnimLayer");
    queue_draw();
  }
}

void
EditorWindow::on_animation_add_keyframe(TimelineProperty property)
{
  std::cout << "EditorWindow::on_animation_add_keyframe()" << std::endl;
  if (Document* document = get_document())
  {
    std::cout << "Document" << std::endl;
    if (TimelineWidget* timeline_widget = get_timeline_widget())
    {
      std::cout << "EditorWindow::on_animation_add_keyframe(): doing" << std::endl;
      SelectionHandle selection = document->get_selection();
      for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
      {
        document->timeline_add_keyframe(*i, property, timeline_widget->get_cursor_pos());
      }
      queue_draw();
    }
  }
}

void
EditorWindow::toggle_render_layer(Glib::RefPtr<Gtk::ToggleAction> action, unsigned int mask)
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wstdisplay::SceneContext& sc = *wst->get_sc();

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
  return m_current_tool;
}

ScrollTool*
EditorWindow::get_scroll_tool() const
{
  return m_scroll_tool.get();
}

TimelineWidget*
EditorWindow::get_timeline_widget()
{
  int page = m_notebook.get_current_page();
  if (page == -1)
  {
    return nullptr;
  }
  else
  {
    if (Gtk::VPaned* paned = dynamic_cast<Gtk::VPaned*>(m_notebook.get_nth_page(page)))
    {
      if (AnimationWidget* animation_widget = dynamic_cast<AnimationWidget*>(paned->get_child2()))
      {
        return &animation_widget->get_timeline_widget();
      }
      else
      {
        return nullptr;
      }
    }
    else
    {
      return nullptr;
    }
  }
}

AnimationWidget*
EditorWindow::get_animation_widget()
{
  int page = m_notebook.get_current_page();
  if (page == -1)
  {
    return nullptr;
  }
  else
  {
    if (Gtk::VPaned* paned = dynamic_cast<Gtk::VPaned*>(m_notebook.get_nth_page(page)))
    {
      return dynamic_cast<AnimationWidget*>(paned->get_child2());
    }
    else
    {
      return nullptr;
    }
  }
}

Document*
EditorWindow::get_document()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    return &wst->get_document();
  }
  else
  {
    return nullptr;
  }
}

WindstilleWidget*
EditorWindow::get_windstille_widget()
{
  int page = m_notebook.get_current_page();
  if (page == -1)
  {
    return nullptr;
  }
  else
  {
    if (Gtk::VPaned* paned = dynamic_cast<Gtk::VPaned*>(m_notebook.get_nth_page(page)))
    {
      return dynamic_cast<WindstilleWidget*>(paned->get_child1());
    }
    else
    {
      return nullptr;
    }
  }
}

void
EditorWindow::on_switch_page(Gtk::Widget* page, guint page_num)
{
  //std::cout << "on_switch_page(" << page << ", " << page_num << ")" << std::endl;

  if (WindstilleWidget* wst = get_windstille_widget())
  {
    if (wst->get_sc()) {
      m_layer_manager.set_model(&wst->get_document().get_sector_model());
      m_layer_widget->update(wst->get_select_mask());

      m_toggle_color_layer->set_active(wst->get_sc()->get_render_mask() & wstdisplay::SceneContext::COLORMAP);
      m_toggle_light_layer->set_active(wst->get_sc()->get_render_mask() & wstdisplay::SceneContext::LIGHTMAP);
      m_toggle_highlight_layer->set_active(wst->get_sc()->get_render_mask() & wstdisplay::SceneContext::HIGHLIGHTMAP);
      m_toggle_control_layer->set_active(wst->get_sc()->get_render_mask() & wstdisplay::SceneContext::CONTROLMAP);

      m_background_layer->set_active(wst->get_draw_background_pattern());
      m_visible_layer->set_active(wst->get_draw_only_active_layer());
      m_grid_layer->set_active(wst->get_enable_grid());

      update_undo_state();
    }
  }
  else
  {
    m_layer_manager.set_model(nullptr);
  }
}

void
EditorWindow::call_with_document(void (Document::*func)())
{
  WindstilleWidget* wst = get_windstille_widget();
  if (wst)
  {
    (wst->get_document().*func)();
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
  if (m_play_action->get_active())
  {
    //std::cout << "Play" << std::endl;
    m_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &EditorWindow::on_timeout),
                                                        50,
                                                        Glib::PRIORITY_DEFAULT);
  }
  else
  {
    //std::cout << "Stop" << std::endl;
    m_timeout_connection.disconnect();
  }
}

void
EditorWindow::on_cut()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    m_clipboard = wst->get_document().get_selection()->clone();
    wst->get_document().selection_delete();
    queue_draw();
  }
}

void
EditorWindow::on_copy()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    m_clipboard = wst->get_document().get_selection()->clone();
  }
}

void
EditorWindow::on_delete_layer()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().layer_remove(wst->get_current_layer_path());
    queue_draw();
  }
}

void
EditorWindow::on_reverse_layers()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().get_sector_model().reverse_layers();
  }
}

void
EditorWindow::on_new_layer()
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().layer_add(wst->get_current_layer_path());
    m_layer_manager.get_treeview().expand_all();
  }
}

void
EditorWindow::on_paste()
{
  if (m_clipboard.get())
  {
    if (WindstilleWidget* wst = get_windstille_widget())
    {
      wst->get_document().undo_group_begin();
      LayerHandle layer = wst->get_current_layer();
      for(Selection::reverse_iterator i = m_clipboard->rbegin(); i != m_clipboard->rend(); ++i)
      {
        wst->get_document().object_add(layer, *i);
      }
      wst->get_document().undo_group_end();

      wst->get_document().set_selection(m_clipboard);
      m_clipboard = m_clipboard->clone();
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
    wst->get_document().select_all();
  }
}

void
EditorWindow::on_show_all(bool v)
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().get_sector_model().set_all_visible(v);
    wst->queue_draw();
  }
}

void
EditorWindow::on_lock_all(bool v)
{
  if (WindstilleWidget* wst = get_windstille_widget())
  {
    wst->get_document().get_sector_model().set_all_locked(v);
    wst->queue_draw();
  }
}

void
EditorWindow::on_recent_file(Glib::RefPtr<Gtk::RecentAction> const& recent_action)
{
  Glib::RefPtr<const Gtk::RecentInfo> item = recent_action->get_current_item();

  //std::cout << "On Recent File:" << item->get_uri() << std::endl;
  if (item->exists())
    load_file(Glib::filename_from_uri(item->get_uri()));
}

void
EditorWindow::add_recent_file(std::string const& filename)
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
  m_statusbar.remove_message(id);
  return false;
}

void
EditorWindow::print(std::string const& text)
{
  guint id = m_statusbar.push(text);
  std::cout << "[LOG] " << text << std::endl;
  Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &EditorWindow::remove_message), id), 6000);
}

void
EditorWindow::print_coordinates(std::string const& text)
{
  m_status_label.set_text(text);
}

/* EOF */
