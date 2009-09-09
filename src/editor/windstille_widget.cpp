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

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtkmm.h>
#include <boost/bind.hpp>

#include "sprite2d/sprite.hpp"
#include "display/display.hpp"
#include "display/compositor.hpp"
#include "display/texture_manager.hpp"
#include "display/surface_manager.hpp"
#include "display/opengl_state.hpp"
#include "display/surface.hpp"
#include "util/pathname.hpp"
#include "editor/editor_window.hpp"
#include "editor/scroll_tool.hpp"
#include "editor/sector_model.hpp"
#include "editor/group_command.hpp"
#include "editor/functor_command.hpp"
#include "editor/document.hpp"
#include "editor/sprite_object_model.hpp"
#include "editor/windstille_widget.hpp"

bool lib_init = false;

WindstilleWidget::WindstilleWidget(EditorWindow& editor_,
                                   const Glib::RefPtr<const Gdk::GL::Config>&  glconfig,
                                   const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  : editor(editor_),
    m_document(new Document),
    filename(),
    state(),
    compositor(),
    sc(),
    scroll_tool(new ScrollTool),
    map_type(DecalObjectModel::COLORMAP),
    background_pattern(),
    select_mask(1),
    draw_background_pattern(true),
    draw_only_active_layers(true),
    grid_enabled(false)
{
  set_gl_capability(glconfig, share_list);

  {
    Glib::RefPtr<Gtk::UIManager>   ui_manager   = editor.get_ui_manager();
    Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();
   
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

  set_flags(get_flags()|Gtk::CAN_FOCUS);

  signal_button_release_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_up));
  signal_button_press_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_down));
  signal_motion_notify_event().connect(sigc::mem_fun(this, &WindstilleWidget::mouse_move));
  signal_scroll_event().connect(sigc::mem_fun(this, &WindstilleWidget::scroll));

  signal_key_press_event().connect(sigc::mem_fun(this, &WindstilleWidget::key_press));
  signal_key_release_event().connect(sigc::mem_fun(this, &WindstilleWidget::key_release));

  //signal_drag_data_received().connect(sigc::mem_fun(this, &WindstilleWidget::on_drag_data_received));
  //signal_drag_finish().connect(sigc::mem_fun(this, &WindstilleWidget::on_drag_finish));

  // Glib::signal_timeout().connect(sigc::mem_fun(this, &WindstilleWidget::on_timeout), 33);
  std::vector<Gtk::TargetEntry> targets;
  targets.push_back(Gtk::TargetEntry("application/x-windstille-decal"));
  drag_dest_set(targets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY);

  m_document->signal_on_change().connect(sigc::mem_fun(this, &WindstilleWidget::on_document_change));
}

WindstilleWidget::~WindstilleWidget()
{
}

bool
WindstilleWidget::on_timeout()
{
  return true;
}

void
WindstilleWidget::on_realize()
{
  Gtk::DrawingArea::on_realize();

  std::cout << "on_realize: " << get_width() << "x" << get_height() << std::endl;

  state.set_size(get_width(), get_height());
  Display::aspect_size.width  = get_width();
  Display::aspect_size.height = get_height();

  std::cout << Display::aspect_size << std::endl;

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (glwindow->gl_begin(get_gl_context()))
    {
      if (!lib_init)
        {
          lib_init = true;
          GLenum err = glewInit();
          if(err != GLEW_OK) {
            std::ostringstream msg;
            msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
            throw std::runtime_error(msg.str());
          }

          OpenGLState::init();
        }
      
      if (!sc.get())
        {
          sc.reset(new SceneContext());
          compositor.reset(new Compositor(Size(get_width(), get_height()),
                                          Size(get_width(), get_height())));
          sc->set_render_mask(sc->get_render_mask() & ~SceneContext::LIGHTMAP);
        }
      
      background_pattern = Texture(Pathname("editor/background_layer.png"));
      background_pattern.set_wrap(GL_REPEAT);

      glViewport(0, 0, get_width(), get_height());
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, get_width(), get_height(), 0.0, 1000.0, -1000.0);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
  
      glwindow->gl_end();
    }
}

bool
WindstilleWidget::on_configure_event(GdkEventConfigure* ev)
{
  Display::aspect_size.width  = ev->width;
  Display::aspect_size.height = ev->height;

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (!glwindow->gl_begin(get_gl_context()))
    {
      return false;
    }
  else
    {
      if (compositor.get())
      {
        compositor.reset(new Compositor(Size(ev->width, ev->height),
                                        Size(ev->width, ev->height)));
      }

      glViewport(0, 0, get_width(), get_height());
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, get_width(), get_height(), 0.0, 1000.0, -1000.0);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
  
      glwindow->gl_end();

      return true;
    }
}

bool
WindstilleWidget::on_expose_event(GdkEventExpose* /*event*/)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (!glwindow->gl_begin(get_gl_context()))
    {
      return false;
    }
  else
    {
      draw();

      // Swap buffers.
      if (glwindow->is_double_buffered())
        glwindow->swap_buffers();
      else
        glFlush();

      glwindow->gl_end();

      return true;
    }
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
  if (sc.get())
    {
      state.push(*sc);
      
      sc->light().fill_screen(m_document->get_sector_model().get_ambient_color());

      if (draw_background_pattern)
        {
          sc->color().fill_pattern(background_pattern, 
                                   state.get_offset() * state.get_zoom());
        }
      else
        {
          sc->color().fill_screen(Color());
        }

      if (draw_only_active_layers)
        m_document->get_sector_model().draw(*sc, SelectMask());
      else
        m_document->get_sector_model().draw(*sc, select_mask);

      if (!m_document->get_selection()->empty())
        {
          for(Selection::iterator i = m_document->get_selection()->begin(); i != m_document->get_selection()->end(); ++i)
            {
              (*i)->draw_select(*sc, i == m_document->get_selection()->begin());
            }

          //sc->control().draw_rect(selection->get_bounding_box(), Color(1.0f, 1.0f, 1.0f, 1.0f));
        }

      for(std::vector<ControlPointHandle>::const_iterator i = m_document->get_control_points().begin();
          i != m_document->get_control_points().end(); ++i)
        {
          (*i)->draw(*sc);
        }

      if (EditorWindow::current()->get_current_tool())
        {
          EditorWindow::current()->get_current_tool()->draw(*sc);
        }

      compositor->render(*sc, &m_document->get_sector_model().get_scene_graph(), state);

      state.pop(*sc);

      if (grid_enabled)
        {
          Display::draw_grid(state.get_offset() * state.get_zoom(),
                             Sizef(128.0f * state.get_zoom(), 128.0f * state.get_zoom()), Color(1,1,1,0.5f));
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
      EditorWindow::current()->get_current_tool()->mouse_down(ev, *this);
      return true;
    }
  else if (ev->button == 2)
    { // Scroll
      scroll_tool->mouse_down(ev, *this);
      return true;
    }
  else if (ev->button == 3)
    { // Context Menu
      EditorWindow::current()->get_current_tool()->mouse_right_down(ev, *this);
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
  
  EditorWindow::current()->get_current_tool()->mouse_move(ev, *this);
  scroll_tool->mouse_move(ev, *this);
  
  return true;
}

bool
WindstilleWidget::mouse_up(GdkEventButton* ev)
{
  //std::cout << "Button Release: " << ev->x << ", " << ev->y << " - " << ev->button << std::endl;
  //viewer->on_mouse_button_up(Vector2i(ev->x, ev->y), ev->button);
  if (ev->button == 1)
    {
      EditorWindow::current()->get_current_tool()->mouse_up(ev, *this);
      queue_draw();
    }
  else if (ev->button == 2)
    {
      scroll_tool->mouse_up(ev, *this);
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
      case GDK_1:
        map_type = DecalObjectModel::COLORMAP;
        EditorWindow::current()->print("COLORMAP");
        break;

      case GDK_2:
        map_type = DecalObjectModel::LIGHTMAP;
        EditorWindow::current()->print("LIGHTMAP");
        break;

      case GDK_3:
        map_type = DecalObjectModel::HIGHLIGHTMAP;
        EditorWindow::current()->print("HIGHLIGHT");
        break;

      case GDK_a:
        EditorWindow::current()->on_select_all();
        break;

      case GDK_d:
        m_document->selection_duplicate();
        break;

      case GDK_s:
        SurfaceManager::current()->save_all_as_png();
        break;

      case GDK_Delete:
        m_document->selection_delete();
        break;

      case GDK_Left:
        state.set_pos(state.get_pos() + Vector2f(-100.0f, 0.0f));
        break;

      case GDK_Right:
        state.set_pos(state.get_pos() + Vector2f(100.0f, 0.0f));
        queue_draw();
        break;

      case GDK_Up:
        state.set_pos(state.get_pos() + Vector2f(0.0f, -100.0f));
        queue_draw();
        break;

      case GDK_Down:
        state.set_pos(state.get_pos() + Vector2f(0.0f, 100.0f));
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
WindstilleWidget::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& /*context*/, int /*x*/, int /*y*/, guint /*time*/)
{
  //std::cout << "WindstilleWidget: on_drag_drop: " << x << ", " << y << ": " << std::endl;
  return true;
}

void
WindstilleWidget::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                                        int x, int y, const Gtk::SelectionData& data,
                                        guint /*info*/, guint /*time*/)
{
  //std::cout << "WindstilleWidget: on_drag_data_received: "
  //          << x << ", " << y << ": " << data.get_data_type() << " " << data.get_data_as_string() << std::endl;
  
  ObjectModelHandle object = DecalObjectModel::create(data.get_data_as_string(),
                                                      state.screen_to_world(Vector2f(static_cast<float>(x), static_cast<float>(y))),
                                                      data.get_data_as_string().substr(5), 
                                                      map_type);

  // if layer mask is 0, set it to all layers instead, so that the
  // object doesn't become unusable
  if (!select_mask)
    object->set_select_mask(SelectMask());
  else
    object->set_select_mask(select_mask);

  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;

  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path_, focus_column);

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
WindstilleWidget::on_drag_finish(const Glib::RefPtr<Gdk::DragContext>& /*context*/)
{
  //std::cout << "WindstilleWidget: on_drag_finish()" << std::endl;
}

void
WindstilleWidget::on_zoom_in()
{
  state.set_zoom(Vector2f(static_cast<float>(get_width())/2.0f, static_cast<float>(get_height())/2.0f), state.get_zoom() * 1.25f);
  queue_draw();
}

void
WindstilleWidget::on_zoom_out()
{
  state.set_zoom(Vector2f(static_cast<float>(get_width())/2.0f, static_cast<float>(get_height())/2.0f), state.get_zoom() * (1.0f/1.25f));
  queue_draw();
}

void
WindstilleWidget::on_zoom_100()
{
  state.set_zoom(Vector2f(static_cast<float>(get_width())  / 2.0f, 
                          static_cast<float>(get_height()) / 2.0f),
                 1.0f);
  queue_draw();
}

LayerHandle
WindstilleWidget::get_current_layer()
{
  Gtk::TreeModel::Path path_;
  Gtk::TreeViewColumn* focus_column;
  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path_, focus_column);

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
  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path_, focus_column);

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
  EditorWindow::current()->update_undo_state();
  m_document->get_sector_model().rebuild_scene_graph();
  queue_draw();
}

void
WindstilleWidget::save_screenshot(const std::string& filename_)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (glwindow->gl_begin(get_gl_context()))
    {
      Display::save_screenshot(Pathname(filename_, Pathname::kSysPath));
      glwindow->gl_end();
    }
}

void
WindstilleWidget::load_file(const std::string& filename_)
{
  filename = filename_;
  m_document.reset(new Document(filename));
}

/* EOF */
