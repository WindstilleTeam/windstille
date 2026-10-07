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


#include "editor/windstille_widget.hpp"
#include "editor/layer_manager_columns.hpp"

#include <cmath>
#include <iostream>

#include <gtkmm.h>

#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/texture_params.hpp>

#include "display/scene_context.hpp"
#include "editor/app.hpp"
#include "editor/gl_device.hpp"
#include "editor/document.hpp"
#include "editor/editor_window.hpp"
#include "editor/functor_command.hpp"
#include "editor/group_command.hpp"
#include "editor/scroll_tool.hpp"
#include "editor/sector_model.hpp"
#include "editor/sprite_object_model.hpp"
#include "sprite2d/sprite.hpp"
#include "util/pathname.hpp"

namespace windstille {

WindstilleWidget::WindstilleWidget(EditorWindow& editor_) :
  m_editor(editor_),
  m_document(new Document),
  filename(),
  m_renderer(),
  m_view(),
  sc(std::make_unique<SceneContext>()),
  m_overlay(),
  map_type(DecalObjectModel::COLORMAP),
  background_pattern(),
  select_mask(1),
  draw_background_pattern(true),
  draw_only_active_layers(true),
  grid_enabled(false)
{
  // the context comes from GLDevice, see on_create_context()
  set_auto_render(true);
  set_has_depth_buffer();
  set_has_stencil_buffer();

  // the editor shows the scene unlit
  sc->set_render_mask(sc->get_render_mask() & ~SceneContext::LIGHTMAP);

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

  m_document->signal_on_change().connect([this]() { on_document_change(); });
}

WindstilleWidget::~WindstilleWidget()
{
  if (m_renderer) {
    g_app.gl_device().get_context()->make_current();
    m_renderer.reset();
  }
}

Glib::RefPtr<Gdk::GLContext>
WindstilleWidget::on_create_context()
{
  // all documents share one context, so that they share the
  // textures, sprites and fonts of the Device
  return g_app.gl_device().get_context();
}

void
WindstilleWidget::on_realize()
{
  Gtk::GLArea::on_realize();
  make_current();
  throw_if_error();

  m_renderer = std::make_unique<wstdisplay::Renderer>(g_app.device());

  background_pattern = g_app.texture().get(Pathname("editor/background_layer.png"),
                                           wstdisplay::TextureParams{
                                             .wrap_x = wstdisplay::TextureWrap::Repeat,
                                             .wrap_y = wstdisplay::TextureWrap::Repeat
                                           });
}

void
WindstilleWidget::on_unrealize()
{
  make_current();
  m_renderer.reset();
  Gtk::GLArea::on_unrealize();
}

bool
WindstilleWidget::on_render(Glib::RefPtr<Gdk::GLContext> const& /*context*/)
{
  throw_if_error();
  if (!m_renderer) {
    return false;
  }

  m_renderer->begin_frame(m_view.get_size());
  m_renderer->clear(surf::Color(0.5f, 0.0f, 0.0f));
  draw();
  sc->render(*m_renderer, m_view);
  m_renderer->render(m_overlay);
  m_overlay.clear();
  m_renderer->end_frame();

  return true;
}

void
WindstilleWidget::on_resize(int width, int height)
{
  GLArea::on_resize(width, height);

  // the size of the framebuffer, larger than the widget on high-DPI
  // screens. The world point in the top left corner stays in place, at
  // the start that is the origin.
  geom::fpoint const top_left = m_view.screen_to_world(geom::fpoint(0.0f, 0.0f));
  m_view.set_size(geom::isize(width, height));
  m_view.set_pos(top_left + geom::foffset(static_cast<float>(width) / 2.0f / m_view.get_zoom(),
                                          static_cast<float>(height) / 2.0f / m_view.get_zoom()));
  queue_draw();
}

glm::vec2
WindstilleWidget::screen_to_world(double x, double y) const
{
  return screen_to_world(m_view, x, y);
}

glm::vec2
WindstilleWidget::screen_to_world(wstdisplay::View const& view, double x, double y) const
{
  float const scale = static_cast<float>(get_scale_factor());
  return view.screen_to_world(geom::fpoint(static_cast<float>(x) * scale,
                                           static_cast<float>(y) * scale)).as_vec();
}

float
WindstilleWidget::get_zoom() const
{
  return m_view.get_zoom() / static_cast<float>(get_scale_factor());
}

void
WindstilleWidget::update(float delta)
{
  std::cout << this << " WindstilleWidget::update(" << delta << ")" << std::endl;
  m_document->get_sector_model().update(delta);
  queue_draw();
}

void
WindstilleWidget::draw()
{
  geom::fsize const size(m_view.get_size());

  sc->light().fill_screen(m_document->get_sector_model().get_ambient_color());

  {
    wstdisplay::Canvas& color = sc->color();
    wstdisplay::Canvas::Scope scope(color);
    color.set_z(-1000.0f);
    color.set_space(wstdisplay::Space::Screen);
    if (draw_background_pattern) {
      // moves with the world but isn't scaled
      geom::fpoint const origin = m_view.world_to_screen(geom::fpoint(0.0f, 0.0f));
      color.fill_pattern(background_pattern, geom::frect(geom::fpoint(0.0f, 0.0f), size),
                         geom::foffset(origin.x(), origin.y()));
    } else {
      color.fill_screen(surf::Color(0.0f, 0.0f, 0.0f));
    }
  }

  m_document->get_sector_model().draw_content(*sc);

  if (draw_only_active_layers) {
    m_document->get_sector_model().draw(*sc, SelectMask());
  } else {
    m_document->get_sector_model().draw(*sc, select_mask);
  }

  if (!m_document->get_selection()->empty()) {
    for(auto it = m_document->get_selection()->begin(); it != m_document->get_selection()->end(); ++it) {
      (*it)->draw_select(*sc, it == m_document->get_selection()->begin());
    }
  }

  for(auto it = m_document->get_control_points().begin();
      it != m_document->get_control_points().end(); ++it) {
    (*it)->draw(m_overlay, m_view);
  }

  if (m_editor.get_current_tool()) {
    m_editor.get_current_tool()->draw(*sc);
  }

  if (grid_enabled) {
    float const step = 128.0f * m_view.get_zoom();
    if (step >= 4.0f) {
      geom::fpoint const origin = m_view.world_to_screen(geom::fpoint(0.0f, 0.0f));
      surf::Color const grid_color(1.0f, 1.0f, 1.0f, 0.75f);
      for (float x = std::fmod(origin.x(), step); x < size.width(); x += step) {
        m_overlay.draw_line(geom::fpoint(x, 0.0f), geom::fpoint(x, size.height()), grid_color);
      }
      for (float y = std::fmod(origin.y(), step); y < size.height(); y += step) {
        m_overlay.draw_line(geom::fpoint(0.0f, y), geom::fpoint(size.width(), y), grid_color);
      }
    }
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
      g_app.surface().get_packer().save_all_as_png(".");
      break;

    case GDK_KEY_Delete:
      m_document->selection_delete();
      break;

    case GDK_KEY_Left:
      m_view.set_pos(m_view.get_pos() + geom::foffset(-100.0f, 0.0f));
      break;

    case GDK_KEY_Right:
      m_view.set_pos(m_view.get_pos() + geom::foffset(100.0f, 0.0f));
      queue_draw();
      break;

    case GDK_KEY_Up:
      m_view.set_pos(m_view.get_pos() + geom::foffset(0.0f, -100.0f));
      queue_draw();
      break;

    case GDK_KEY_Down:
      m_view.set_pos(m_view.get_pos() + geom::foffset(0.0f, 100.0f));
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
WindstilleWidget::on_drag_drop(Glib::RefPtr<Gdk::DragContext> const& context, int x, int y, guint time)
{
  std::cout << "WindstilleWidget: on_drag_drop: " << x << ", " << y << ": " << std::endl;
  return true;
}

void
WindstilleWidget::on_drag_data_received(Glib::RefPtr<Gdk::DragContext> const& /*context*/,
                                        int x, int y, Gtk::SelectionData const& data,
                                        guint info, guint time)
{
  std::cout << "WindstilleWidget: on_drag_data_received: "
            << x << ", " << y << ": " << data.get_data_type() << " " << data.get_data_as_string() << std::endl;

  ObjectModelHandle object = DecalObjectModel::create(data.get_data_as_string(),
                                                      screen_to_world(x, y),
                                                      data.get_data_as_string(),
                                                      map_type);

  // if layer mask is 0, set it to all layers instead, so that the
  // object doesn't become unusable
  if (!select_mask)
    object->set_select_mask(SelectMask());
  else
    object->set_select_mask(select_mask);

  LayerHandle layer = get_current_layer();
  if (!layer) {
    std::cout << "WindstilleWidget::on_drag_data_received(): Error: no current layer" << std::endl;
  } else {
    get_document().object_add(layer, object);
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
  geom::fsize const size(m_view.get_size());
  m_view.set_zoom(geom::fpoint(size.width() / 2.0f, size.height() / 2.0f), m_view.get_zoom() * 1.25f);
  queue_draw();
}

void
WindstilleWidget::on_zoom_out()
{
  geom::fsize const size(m_view.get_size());
  m_view.set_zoom(geom::fpoint(size.width() / 2.0f, size.height() / 2.0f), m_view.get_zoom() / 1.25f);
  queue_draw();
}

void
WindstilleWidget::on_zoom_100()
{
  // one world pixel per widget pixel
  geom::fsize const size(m_view.get_size());
  m_view.set_zoom(geom::fpoint(size.width() / 2.0f, size.height() / 2.0f), static_cast<float>(get_scale_factor()));
  queue_draw();
}

LayerHandle
WindstilleWidget::get_current_layer()
{
  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;
  m_editor.get_layer_manager().get_treeview().get_cursor(path_, focus_column);
  if (!path_.gobj()) {
    std::cout << "WindstilleWidget::get_current_layer(): Error: Couldn't get path" << std::endl;
    return LayerHandle();
  }
  auto it = m_editor.get_layer_manager().get_treeview().get_model()->get_iter(path_);
  if (!it) return LayerHandle();
  return (*it)[LayerManagerColumns::instance().layer];
}

void
WindstilleWidget::on_document_change()
{
  m_editor.update_undo_state();
  queue_draw();
}

void
WindstilleWidget::save_screenshot(std::string const& filename_)
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
WindstilleWidget::load_file(std::string const& filename_)
{
  filename = filename_;
  m_document.reset(new Document(filename));
  m_document->signal_on_change().connect([this]() { on_document_change(); });
  on_document_change();
}


} // namespace windstille

/* EOF */
