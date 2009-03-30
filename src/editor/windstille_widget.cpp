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

#include "sprite2d/sprite.hpp"
#include "display/display.hpp"
#include "display/texture_manager.hpp"
#include "display/surface_manager.hpp"
#include "display/opengl_state.hpp"
#include "display/surface.hpp"
#include "editor_window.hpp"
#include "scroll_tool.hpp"
#include "sector_model.hpp"

#include "sprite_object_model.hpp"
#include "windstille_widget.hpp"

bool lib_init = false;

WindstilleWidget::WindstilleWidget(const Glib::RefPtr<const Gdk::GL::Config>&  glconfig,
                                   const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  : sector_model(new SectorModel()),
    active_tool(0),
    scroll_tool(new ScrollTool()),
    map_type(DecalObjectModel::COLORMAP),
    draw_background_pattern(true),
    layer_mask(1),
    draw_only_active_layers(true),
    grid_enabled(false)
{
  set_gl_capability(glconfig, share_list);

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
  targets.push_back(Gtk::TargetEntry("WindstilleObject"));
  drag_dest_set(targets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY);

  selection = Selection::create();
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
  state.set_size(get_width(), get_height());

  //std::cout << "WindstilleWidget::on_realize()" << std::endl;

  Gtk::DrawingArea::on_realize();

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
          //texture_manager  = new TextureManager();
          new SurfaceManager();
          //sprite2d_manager = new SpriteManager();
        }
      
      if (!sc.get())
        {
          sc.reset(new SceneContext());
          sc->set_render_mask(sc->get_render_mask() & ~SceneContext::LIGHTMAP);
        }
      
      background_pattern = Texture("editor/background_layer.png");
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
WindstilleWidget::on_configure_event(GdkEventConfigure* event)
{
  state.set_size(get_width(), get_height());
  Display::aspect_size.width  = get_width();
  Display::aspect_size.height = get_height();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  // *** OpenGL BEGIN ***
  if (!glwindow->gl_begin(get_gl_context()))
    {
      return false;
    }
  else
    {
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
WindstilleWidget::on_expose_event(GdkEventExpose* event)
{
  //std::cout << "WindstilleWidget::on_expose()" << std::endl;
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
  sector_model->update(delta);
  queue_draw();
}

void
WindstilleWidget::draw()
{
  //std::cout << "Draw" << std::endl;
  if (sc.get())
    {
      state.push(*sc);
      
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
        sector_model->draw(*sc, Layers());
      else
        sector_model->draw(*sc, layer_mask);

      for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
        {
          if (i == selection->begin())
            sc->control().draw_rect((*i)->get_bounding_box(), Color(1.0f, 1.0f, 1.0f, 1.0f));
          else
            sc->control().draw_rect((*i)->get_bounding_box(), Color(0.5f, 0.5f, 1.0f, 1.0f));
        }

      if (active_tool)
        active_tool->draw(*sc);

      sc->render();
      state.pop(*sc);

      if (grid_enabled)
        {
          Display::draw_grid(state.get_offset() * state.get_zoom(),
                             Sizef(128.0f * state.get_zoom(), 128.0f * state.get_zoom()), Color(1,1,1,0.5f));
        }
    }
}

void
WindstilleWidget::selection_raise()
{
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      sector_model->raise(*i);
    }
  queue_draw();
}

void
WindstilleWidget::selection_lower()
{
  for(Selection::reverse_iterator i = selection->rbegin(); i != selection->rend(); ++i)
    {
      sector_model->lower(*i);
    }
  queue_draw();
}

void
WindstilleWidget::selection_raise_to_top()
{
  for(Selection::reverse_iterator i = selection->rbegin(); i != selection->rend(); ++i)
    {
      sector_model->raise_to_top(*i);
    }
  queue_draw();
}

void
WindstilleWidget::selection_lower_to_bottom()
{
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      sector_model->lower_to_bottom(*i);
    }
  queue_draw();
}

void
WindstilleWidget::selection_connect_parent()
{
  if (selection->size() >= 2)
    {
      ObjectModelHandle parent = *selection->begin();

      Selection::iterator i = selection->begin();
      ++i;
      for(; i != selection->end(); ++i)
        {
          (*i)->set_parent(parent);
        }
    }
  queue_draw();
}

void
WindstilleWidget::selection_clear_parent()
{
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      (*i)->set_parent(ObjectModelHandle());
    }
  queue_draw();
}

void
WindstilleWidget::selection_duplicate()
{
  std::map<ObjectModelHandle, ObjectModelHandle> parent_map;

  SelectionHandle new_selection = Selection::create();
  for(Selection::reverse_iterator i = selection->rbegin(); i != selection->rend(); ++i)
    {
      HardLayerHandle layer = sector_model->get_layer(*i);
      ObjectModelHandle obj = (*i)->clone();

      parent_map[*i] = obj;

      if (!layer)
        {
          EditorWindow::current()->print("Couldn't find parent layer while duplicating");
        }
      else
        {
          // Move clone a litte to make it more obvious that something happened
          obj->set_rel_pos(obj->get_rel_pos() + Vector2f(32.0f, 32.0f));
          layer->add(obj);
          new_selection->add(obj);
        }
    }

  selection = new_selection;

  // Second pass to set the parents to the cloned objects
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      if ((*i)->get_parent())
        {
          std::map<ObjectModelHandle, ObjectModelHandle>::iterator it = parent_map.find((*i)->get_parent());
          
          if (it == parent_map.end())
            {
              // When the parent wasn't part of the selection, leave
              // the parent untouched
            }
          else
            {
              (*i)->set_parent(it->second);
            }
        }
    }

  queue_draw();
}

void
WindstilleWidget::selection_delete()
{
 for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      sector_model->remove(*i);
    } 
 selection->clear();

 queue_draw();
}

bool
WindstilleWidget::scroll(GdkEventScroll* event)
{
  if (event->direction == GDK_SCROLL_UP)
    {
      //viewer->get_state().zoom(1.1f, Vector2i(event->x, event->y));
    }
  else if (event->direction == GDK_SCROLL_DOWN)
    {
      //viewer->get_state().zoom(1.0f/1.1f, Vector2i(event->x, event->y));
    }
  return false;
}

bool
WindstilleWidget::mouse_down(GdkEventButton* event)
{
  grab_focus();
  //std::cout << "Button Press: " << event->x << ", " << event->y << " - " << event->button << std::endl;
  //ewer->on_mouse_button_down(Vector2i(event->x, event->y), event->button);
  if (event->button == 1)
    {
      active_tool = EditorWindow::current()->get_current_tool();
      active_tool->mouse_down(event, *this);
    }
  else if (event->button == 2)
    {
      active_tool = scroll_tool.get();
      active_tool->mouse_down(event, *this);
    }

  return false;
}

bool
WindstilleWidget::mouse_move(GdkEventMotion* event)
{
  //std::cout << "Motion: " << event->x << ", " << event->y << std::endl;
  
  if (active_tool)
    {
      active_tool->mouse_move(event, *this);
      queue_draw();
    }
  
  return true;
}

bool
WindstilleWidget::mouse_up(GdkEventButton* event)
{
  active_tool = 0;

  //std::cout << "Button Release: " << event->x << ", " << event->y << " - " << event->button << std::endl;
  //viewer->on_mouse_button_up(Vector2i(event->x, event->y), event->button);
  if (event->button == 1)
    {
      EditorWindow::current()->get_current_tool()->mouse_up(event, *this);
      queue_draw();
    }
  else if (event->button == 2)
    {
      scroll_tool->mouse_up(event, *this);
      queue_draw();
    }
  return false;
}

bool
WindstilleWidget::key_press(GdkEventKey* event)
{
  switch(event->keyval)
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

      case GDK_Left:
        state.set_pos(state.get_pos() + Vector2f(-100.0f, 0.0f));
        queue_draw();
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

  //std::cout << state.get_pos() << std::endl;

  return true;
}

bool
WindstilleWidget::key_release(GdkEventKey* event)
{ // /usr/include/gtk-2.0/gdk/gdkkeysyms.h
  //std::cout << "KeyRelease: " << (int)event->keyval << std::endl;
  return true;
}

bool
WindstilleWidget::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  //std::cout << "WindstilleWidget: on_drag_drop: " << x << ", " << y << ": " << std::endl;
  return true;
}

void
WindstilleWidget::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
                                        int x, int y, const Gtk::SelectionData& data,
                                        guint info, guint time)
{
  //std::cout << "WindstilleWidget: on_drag_data_received: "
  //          << x << ", " << y << ": " << data.get_data_type() << " " << data.get_data_as_string() << std::endl;
  
  ObjectModelHandle object = DecalObjectModel::create(data.get_data_as_string(),
                                                      state.screen_to_world(Vector2f(x, y)),
                                                      data.get_data_as_string().substr(5), 
                                                      map_type);

  // if layer mask is 0, set it to all layers instead, so that the
  // object doesn't become unusable
  if (!layer_mask)
    object->set_layers(Layers());
  else
    object->set_layers(layer_mask);

  Gtk::TreeModel::Path path;
  Gtk::TreeViewColumn* focus_column;

  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path, focus_column);

  sector_model->add(object, path);
}

void
WindstilleWidget::on_drag_finish(const Glib::RefPtr<Gdk::DragContext>& context)
{
  //std::cout << "WindstilleWidget: on_drag_finish()" << std::endl;
}

void
WindstilleWidget::on_zoom_in()
{
  state.set_zoom(Vector2f(get_width()/2, get_height()/2), state.get_zoom() * 1.25f);
  queue_draw();
}

void
WindstilleWidget::on_zoom_out()
{
  state.set_zoom(Vector2f(get_width()/2, get_height()/2), state.get_zoom() * (1.0f/1.25f));
  queue_draw();
}

void
WindstilleWidget::on_zoom_100()
{
  state.set_zoom(Vector2f(get_width()/2, get_height()/2), 1.0f);
  queue_draw();
}

void
WindstilleWidget::set_selection(const SelectionHandle& selection_)
{
  selection = selection_;
}

SelectionHandle
WindstilleWidget::get_selection() const
{
  return selection;
}

HardLayerHandle
WindstilleWidget::get_current_layer()
{
  Gtk::TreeModel::Path path;
  Gtk::TreeViewColumn* focus_column;
  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path, focus_column);

  return sector_model->get_layer(path);  
}

Gtk::TreeModel::Path
WindstilleWidget::get_current_layer_path()
{
  Gtk::TreeModel::Path path;
  Gtk::TreeViewColumn* focus_column;
  EditorWindow::current()->get_layer_manager().get_treeview().get_cursor(path, focus_column);
  return path;
}

SectorModel*
WindstilleWidget::get_sector_model()
{
  return sector_model.get();
}

void
WindstilleWidget::load_file(const std::string& filename_)
{
  filename = filename_;
  sector_model->load(filename);
  queue_draw();
}

/* EOF */
