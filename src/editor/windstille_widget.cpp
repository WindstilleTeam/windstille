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

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtkmm.h>


#include <wstdisplay/assert_gl.hpp>
#include <wstdisplay/compositor.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_state.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include "editor/app.hpp"
#include "editor/document.hpp"
#include "editor/editor_window.hpp"
#include "editor/functor_command.hpp"
#include "editor/group_command.hpp"
#include "editor/scroll_tool.hpp"
#include "editor/sector_model.hpp"
#include "editor/sprite_object_model.hpp"
#include "editor/windstille_widget.hpp"
#include <wstdisplay/scenegraph/scene_graph.hpp>
#include "sprite2d/sprite.hpp"
#include "util/pathname.hpp"

namespace {

bool lib_init = false;

} // namespace


WindstilleWidget::WindstilleWidget(EditorWindow& editor_) :
  m_editor(editor_),
  m_gc(),
  m_document(new Document),
  m_scene_graph(new wstdisplay::SceneGraph()),
  m_rebuild_scene_graph(true),
  filename(),
  state(),
  compositor(),
  sc(),
  map_type(DecalObjectModel::COLORMAP),
  background_pattern(),
  select_mask(1),
  draw_background_pattern(true),
  draw_only_active_layers(true),
  grid_enabled(false)
{
  // OpenGL setup
  //get_context().set_debug_enabled(true);
  set_auto_render(true);
  //set_forward_compatible(true);
  set_required_version(3, 3);
  set_has_depth_buffer();
  set_has_stencil_buffer();
  set_has_alpha();
  set_use_es (true);

  {
    Glib::RefPtr<Gtk::UIManager>   ui_manager   = m_editor.get_ui_manager();
    Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create("WindstilleWidget");

    action_group->add(Gtk::Action::create("PopupMenu",   "_PopupMenu"));
    //action_group->add(Gtk::Action::create("ObjectReset", Gtk::Stock::REFRESH));

    ui_manager->insert_action_group(action_group);

    ui_manager->add_ui_from_string("<ui>"
                                   "  <popup name='PopupMenu'>"
                                   "    <menuitem action='Duplicate'/>"
                                   "    <menuitem action='Delete'/>"
                                   "    <separator/>"
                                   "    <menuitem action='ConnectParent'/>"
                                   "    <menuitem action='ClearParent'/>"
                                   "    <separator/>"
                                   "    <menuitem action='HFlipObject'/>"
                                   "    <menuitem action='VFlipObject'/>"
                                   "    <separator/>"
                                   "    <menuitem action='ResetRotation'/>"
                                   "    <menuitem action='ResetScale'/>"
                                   "    <separator/>"
                                   "    <menuitem action='ObjectProperties'/>"
                                   //"    <menuitem action='ObjectReset'/>"
                                   "  </popup>"
                                   "</ui>");
  }

  add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::KEY_PRESS_MASK      | Gdk::KEY_RELEASE_MASK |
             Gdk::ENTER_NOTIFY_MASK   | Gdk::LEAVE_NOTIFY_MASK);

  // Gdk::POINTER_MOTION_HINT_MASK |
  // Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
  // Gdk::BUTTON3_MOTION_MASK |

  set_can_focus();

  signal_button_release_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_up));
  signal_button_press_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_down));
  signal_motion_notify_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_move));
  signal_scroll_event().connect(sigc::mem_fun(this, &WindstilleWidget::scroll));

  signal_key_press_event().connect(sigc::mem_fun(this, &WindstilleWidget::key_press));
  signal_key_release_event().connect(sigc::mem_fun(this, &WindstilleWidget::key_release));

  //signal_drag_data_received().connect(sigc::mem_fun(this, &WindstilleWidget::on_drag_data_received));
  //signal_drag_finish().connect(sigc::mem_fun(this, &WindstilleWidget::on_drag_finish));

  std::vector<Gtk::TargetEntry> targets;
  targets.push_back(Gtk::TargetEntry("application/x-windstille-decal"));
  drag_dest_set(targets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY);

  m_document->signal_on_change().connect(sigc::mem_fun(*this, &WindstilleWidget::on_document_change));
}

WindstilleWidget::~WindstilleWidget()
{
}

Glib::RefPtr<Gdk::GLContext>
WindstilleWidget::on_create_context()
{
  std::cout << "WindstilleWidget::on_create_context():" << std::endl;
  auto ctx = Gtk::GLArea::on_create_context();
  ctx->set_debug_enabled();
  ctx->make_current();

  int major, minor;
  ctx->get_version(major, minor);
  std::cout << major << "." << minor << std::endl;
  std::cout << "ES:" << ctx->get_use_es() << std::endl;
  return ctx;
}

void
WindstilleWidget::on_realize()
{
  Gtk::GLArea::on_realize();

  if (!lib_init)
  {
    lib_init = true;

    if (auto ctx = Gdk::GLContext::get_current()) {
      int major, minor;
      ctx->get_version(major, minor);
      std::cout << "OpenGL Version: " << major << "." << minor << std::endl;
      std::cout << "OpenGLES:" << ctx->get_use_es() << std::endl;
    }

    GLenum err = glewInit();
    if(err != GLEW_OK) {
      std::ostringstream msg;
      msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
      throw std::runtime_error(msg.str());
    }
    assert_gl();
  }

  wstdisplay::OpenGLState::init();
  m_gc = std::make_unique<wstdisplay::GraphicsContext>();
  sc = std::make_unique<wstdisplay::SceneContext>();

  background_pattern = g_app.texture().get(Pathname("editor/background_layer.png"));
  background_pattern->set_wrap(GL_REPEAT);
}

void
WindstilleWidget::on_unrealize()
{
  Gtk::GLArea::on_unrealize();
}

bool
WindstilleWidget::on_render(Glib::RefPtr<Gdk::GLContext> const& context)
{
  Gtk::GLArea::on_render(context);
  throw_if_error();

  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw(*m_gc);

  return false;
}

void
WindstilleWidget::on_resize(int width, int height)
{
  GLArea::on_resize(width, height);

  if (!compositor || compositor->get_framebuffer_size() != geom::isize(width, height)) {
    assert_gl();

    compositor = std::make_unique<wstdisplay::Compositor>(geom::isize(width, height),
                                              geom::isize(width, height));
    sc->set_render_mask(sc->get_render_mask() & ~wstdisplay::SceneContext::LIGHTMAP);

    throw_if_error();
  }

  glViewport(0, 0, width, height);
  //m_gc->set_aspect_size({width, height});
  state.set_size(width, height);
  m_gc->set_ortho({width, height});

  queue_draw();

  throw_if_error();
}

void
WindstilleWidget::update(float delta)
{
  std::cout << this << " WindstilleWidget::update(" << delta << ")" << std::endl;
  m_document->get_sector_model().update(delta);
  queue_draw();
}

void
WindstilleWidget::draw(wstdisplay::GraphicsContext& gc)
{
  if (!sc) { return; }

  if ((true)) { // FIXME: always rebuild for now, optimize later
    m_rebuild_scene_graph = false;
    m_document->get_sector_model().rebuild_scene_graph(*m_scene_graph->get_root());
  }

  state.push(*sc);

  sc->light().fill_screen(m_document->get_sector_model().get_ambient_color());

  if (draw_background_pattern) {
    sc->color().fill_pattern(background_pattern,
                             state.get_offset() * state.get_zoom());
  } else {
    sc->color().fill_screen(surf::Color());
  }

  if (draw_only_active_layers) {
    m_document->get_sector_model().draw(*sc, SelectMask());
  } else {
    m_document->get_sector_model().draw(*sc, select_mask);
  }

  if (!m_document->get_selection()->empty()) {
    for(auto it = m_document->get_selection()->begin(); it != m_document->get_selection()->end(); ++it) {
      (*it)->draw_select(*sc, it == m_document->get_selection()->begin());
    }
    //sc->control().draw_rect(selection->get_bounding_box(), surf::Color(1.0f, 1.0f, 1.0f, 1.0f));
  }

  for(auto it = m_document->get_control_points().begin();
      it != m_document->get_control_points().end(); ++it) {
    (*it)->draw(*sc);
  }

  if (m_editor.get_current_tool()) {
    m_editor.get_current_tool()->draw(*sc);
  }

  compositor->render(gc, *sc, m_scene_graph.get(), state);

  state.pop(*sc);

  if (grid_enabled) {
    gc.draw_grid(state.get_offset().as_vec() * state.get_zoom(),
                 geom::fsize(128.0f * state.get_zoom(), 128.0f * state.get_zoom()),
                 surf::Color(1,1,1,0.75f));
  }
}

bool
WindstilleWidget::scroll(GdkEventScroll* ev)
{
  if (ev->direction == GDK_SCROLL_UP)
  {
    //viewer->get_state().zoom(1.1f, Vector2i(event->x, event->y));
  }
  else if (ev->direction == GDK_SCROLL_DOWN)
  {
    //viewer->get_state().zoom(1.0f/1.1f, Vector2i(event->x, event->y));
  }
  return false;
}

bool
WindstilleWidget::mouse_down(GdkEventButton* ev)
{
  grab_focus();

  //std::cout << "Button Press: " << ev->x << ", " << ev->y << " - " << ev->button << std::endl;

  if (ev->button == 1)
  { // Tool
    m_editor.get_current_tool()->mouse_down(ev, *this);
    return true;
  }
  else if (ev->button == 2)
  { // Scroll
    m_editor.get_scroll_tool()->mouse_down(ev, *this);
    return true;
  }
  else if (ev->button == 3)
  { // Context Menu
    m_editor.get_current_tool()->mouse_right_down(ev, *this);
    return true;
  }
  else
  {
    return false;
  }
}

bool
WindstilleWidget::mouse_move(GdkEventMotion* ev)
{
  //std::cout << "Motion: " << ev->x << ", " << ev->y << std::endl;

  m_editor.get_current_tool()->mouse_move(ev, *this);
  m_editor.get_scroll_tool()->mouse_move(ev, *this);

  return true;
}

bool
WindstilleWidget::mouse_up(GdkEventButton* ev)
{
  //std::cout << "Button Release: " << ev->x << ", " << ev->y << " - " << ev->button << std::endl;
  //viewer->on_mouse_button_up(Vector2i(ev->x, ev->y), ev->button);
  if (ev->button == 1)
  {
    m_editor.get_current_tool()->mouse_up(ev, *this);
    queue_draw();
  }
  else if (ev->button == 2)
  {
    m_editor.get_scroll_tool()->mouse_up(ev, *this);
    queue_draw();
  }

  return false;
}

bool
WindstilleWidget::key_press(GdkEventKey* ev)
{
  //std::cout << ev->keyval << " keypress " << state.get_pos() << std::endl;

  switch(ev->keyval)
  {
    case GDK_KEY_1:
      map_type = DecalObjectModel::COLORMAP;
      m_editor.print("COLORMAP");
      break;

    case GDK_KEY_2:
      map_type = DecalObjectModel::LIGHTMAP;
      m_editor.print("LIGHTMAP");
      break;

    case GDK_KEY_3:
      map_type = DecalObjectModel::HIGHLIGHTMAP;
      m_editor.print("HIGHLIGHT");
      break;

    case GDK_KEY_a:
      m_editor.on_select_all();
      break;

    case GDK_KEY_i:
      std::cout << "Position Keyframe" << std::endl;
      m_editor.on_animation_add_keyframe(kPosition);
      break;

    case GDK_KEY_k:
      std::cout << "Position Scale" << std::endl;
      m_editor.on_animation_add_keyframe(kScale);
      break;

    case GDK_KEY_u:
      std::cout << "Position Rotation" << std::endl;
      m_editor.on_animation_add_keyframe(kRotation);
      break;

    case GDK_KEY_d:
      m_document->selection_duplicate();
      break;

    case GDK_KEY_s:
      g_app.surface().save_all_as_png();
      break;

    case GDK_KEY_F5: // force a rebuild of the scenegraph
      m_rebuild_scene_graph = true;
      queue_draw();
      break;

    case GDK_KEY_Delete:
      m_document->selection_delete();
      break;

    case GDK_KEY_Left:
      state.set_pos(state.get_pos().as_vec() + glm::vec2(-100.0f, 0.0f));
      break;

    case GDK_KEY_Right:
      state.set_pos(state.get_pos().as_vec() + glm::vec2(100.0f, 0.0f));
      queue_draw();
      break;

    case GDK_KEY_Up:
      state.set_pos(state.get_pos().as_vec() + glm::vec2(0.0f, -100.0f));
      queue_draw();
      break;

    case GDK_KEY_Down:
      state.set_pos(state.get_pos().as_vec() + glm::vec2(0.0f, 100.0f));
      queue_draw();
      break;
  }

  return true;
}

bool
WindstilleWidget::key_release(GdkEventKey* ev)
{ // /usr/include/gtk-2.0/gdk/gdkkeysyms.h
  //std::cout << "KeyRelease: " << (int)ev->keyval << std::endl;
  return true;
}

bool
WindstilleWidget::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  std::cout << "WindstilleWidget: on_drag_drop: " << x << ", " << y << ": " << std::endl;
  return true;
}

void
WindstilleWidget::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                                        int x, int y, const Gtk::SelectionData& data,
                                        guint info, guint time)
{
  std::cout << "WindstilleWidget: on_drag_data_received: "
            << x << ", " << y << ": " << data.get_data_type() << " " << data.get_data_as_string() << std::endl;

  ObjectModelHandle object = DecalObjectModel::create(data.get_data_as_string(),
                                                      state.screen_to_world(glm::vec2(static_cast<float>(x), static_cast<float>(y))).as_vec(),
                                                      data.get_data_as_string(),
                                                      map_type);

  // if layer mask is 0, set it to all layers instead, so that the
  // object doesn't become unusable
  if (!select_mask)
    object->set_select_mask(SelectMask());
  else
    object->set_select_mask(select_mask);

  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;

  m_editor.get_layer_manager().get_treeview().get_cursor(path_, focus_column);

  if (!path_.gobj())
  {
    std::cout << "WindstilleWidget::on_drag_data_received(): Error: Couldn't get path" << std::endl;
  }
  else
  {
    get_document().object_add(m_document->get_sector_model().get_layer(path_), object);
  }
}

void
WindstilleWidget::on_drag_end(Glib::RefPtr<Gdk::DragContext> const& context)
{
  std::cout << "WindstilleWidget: on_drag_end()" << std::endl;
}

void
WindstilleWidget::on_zoom_in()
{
  state.set_zoom(glm::vec2(static_cast<float>(get_width())/2.0f, static_cast<float>(get_height())/2.0f), state.get_zoom() * 1.25f);
  queue_draw();
}

void
WindstilleWidget::on_zoom_out()
{
  state.set_zoom(glm::vec2(static_cast<float>(get_width())/2.0f, static_cast<float>(get_height())/2.0f), state.get_zoom() * (1.0f/1.25f));
  queue_draw();
}

void
WindstilleWidget::on_zoom_100()
{
  state.set_zoom(glm::vec2(static_cast<float>(get_width())  / 2.0f,
                          static_cast<float>(get_height()) / 2.0f),
                 1.0f);
  queue_draw();
}

LayerHandle
WindstilleWidget::get_current_layer()
{
  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;
  m_editor.get_layer_manager().get_treeview().get_cursor(path_, focus_column);

  if (!path_.gobj())
  {
    std::cout << "WindstilleWidget::get_current_layer(): Error: Couldn't get path" << std::endl;
    return LayerHandle();
  }
  else
  {
    return m_document->get_sector_model().get_layer(path_);
  }
}

Gtk::TreeModel::Path
WindstilleWidget::get_current_layer_path()
{
  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;
  m_editor.get_layer_manager().get_treeview().get_cursor(path_, focus_column);

  if (!path_.gobj())
  {
    std::cout << "WindstilleWidget::get_current_layer_path(): Error: Couldn't get path" << std::endl;
    return Gtk::TreeModel::Path();
  }
  else
  {
    return path_;
  }
}

void
WindstilleWidget::on_document_change()
{
  m_rebuild_scene_graph = true;
  m_editor.update_undo_state();
  queue_draw();
}

void
WindstilleWidget::save_screenshot(const std::string& filename_)
{
#ifdef FIXME_DISABLED_FOR_GTKMM3_PORT
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (glwindow->gl_begin(get_gl_context()))
  {
    save_screenshot(filename_);
    glwindow->gl_end();
  }
#endif
}

void
WindstilleWidget::load_file(const std::string& filename_)
{
  filename = filename_;
  m_document.reset(new Document(filename));
  m_document->signal_on_change().connect(sigc::mem_fun(*this, &WindstilleWidget::on_document_change));
  on_document_change();
}

/* EOF */
