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
#include "editor/undo_manager.hpp"
#include "editor/group_command.hpp"
#include "editor/object_commands.hpp"
#include "editor/functor_command.hpp"

#include "editor/sprite_object_model.hpp"
#include "editor/windstille_widget.hpp"

bool lib_init = false;

WindstilleWidget::WindstilleWidget(EditorWindow& editor_,
                                   const Glib::RefPtr<const Gdk::GL::Config>&  glconfig,
                                   const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  : editor(editor_),
    undo_manager(new UndoManager()),
    filename(),
    sector_model(new SectorModel()),
    control_points(),
    state(),
    compositor(),
    sc(),
    scroll_tool(new ScrollTool()),
    selection(),
    map_type(DecalObjectModel::COLORMAP),
    background_pattern(),
    draw_background_pattern(true),
    select_mask(1),
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

  set_selection(Selection::create());
}

WindstilleWidget::~WindstilleWidget()
{
}

void
WindstilleWidget::execute(CommandHandle cmd)
{
  undo_manager->execute(cmd);
  EditorWindow::current()->update_undo_state();
  queue_draw();
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
  sector_model->update(delta);
  queue_draw();
}

void
WindstilleWidget::draw()
{
  if (sc.get())
    {
      state.push(*sc);
      
      sc->light().fill_screen(sector_model->get_ambient_color());

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
        sector_model->draw(*sc, SelectMask(), editor.get_draw_navgraph());
      else
        sector_model->draw(*sc, select_mask, editor.get_draw_navgraph());

      if (!selection->empty())
        {
          for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
            {
              (*i)->draw_select(*sc, i == selection->begin());
            }

          //sc->control().draw_rect(selection->get_bounding_box(), Color(1.0f, 1.0f, 1.0f, 1.0f));
        }

      for(std::vector<ControlPointHandle>::iterator i = control_points.begin();
          i != control_points.end(); ++i)
        {
          (*i)->draw(*sc);
        }

      if (EditorWindow::current()->get_current_tool())
        {
          EditorWindow::current()->get_current_tool()->draw(*sc);
        }

      compositor->render(*sc, sector_model->get_scene_graph(), state);

      state.pop(*sc);

      if (grid_enabled)
        {
          Display::draw_grid(state.get_offset() * state.get_zoom(),
                             Sizef(128.0f * state.get_zoom(), 128.0f * state.get_zoom()), Color(1,1,1,0.5f));
        }
    }
}

void
WindstilleWidget::undo()
{
  undo_manager->undo();
}

void
WindstilleWidget::redo()
{
  undo_manager->redo();
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
WindstilleWidget::selection_vflip()
{
  if (!selection->empty())
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      if (selection->size() > 1)
        {
          const Vector2f& center = selection->get_bounding_box().get_center();
          for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
            {
              Vector2f pos = (*i)->get_world_pos();
          
              pos.y = center.y + (center.y - pos.y);
          
              //(*i)->set_world_pos(pos);
              group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_world_pos, *i, (*i)->get_world_pos()),
                                                                  boost::bind(&ObjectModel::set_world_pos, *i, pos))));
            }
        }

      for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
        {
          //(*i)->set_vflip(!(*i)->get_vflip());
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_vflip, *i,  (*i)->get_vflip()),
                                                              boost::bind(&ObjectModel::set_vflip, *i, !(*i)->get_vflip()))));
        }

      execute(group_command);
    }
}

void
WindstilleWidget::selection_hflip()
{
  if (!selection->empty())
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      if (selection->size() > 1)
        {
          const Vector2f& center = selection->get_bounding_box().get_center();
          for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
            {
              Vector2f pos = (*i)->get_world_pos();
          
              pos.x = center.x + (center.x - pos.x);
          
              //(*i)->set_world_pos(pos);
              group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_world_pos, *i, (*i)->get_world_pos()),
                                                                  boost::bind(&ObjectModel::set_world_pos, *i, pos))));
            }
        }

      for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
        {
          //(*i)->set_hflip(!(*i)->get_hflip());
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_hflip, *i,  (*i)->get_hflip()),
                                                              boost::bind(&ObjectModel::set_hflip, *i, !(*i)->get_hflip()))));
        }

      execute(group_command);
    }
}

void
WindstilleWidget::selection_connect_parent()
{
  if (selection->size() >= 2)
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      ObjectModelHandle parent = *selection->begin();

      Selection::iterator i = selection->begin();
      ++i;
      for(; i != selection->end(); ++i)
        {
          //(*i)->set_parent(parent);
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_parent, *i, (*i)->get_parent(), true),
                                                              boost::bind(&ObjectModel::set_parent, *i, parent, true))));
        }

      execute(group_command);
    }
}

void
WindstilleWidget::selection_clear_parent()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      //(*i)->set_parent(ObjectModelHandle());
      group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_parent, *i, (*i)->get_parent(), true),
                                                          boost::bind(&ObjectModel::set_parent, *i, ObjectModelHandle(), true))));
    }

  execute(group_command);
}

void
WindstilleWidget::selection_duplicate()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());
  std::map<ObjectModelHandle, ObjectModelHandle> parent_map;

  SelectionHandle new_selection = Selection::create();
  for(Selection::reverse_iterator i = selection->rbegin(); i != selection->rend(); ++i)
    {
      LayerHandle layer = sector_model->get_layer(*i);
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
          new_selection->add(obj);

          //layer->add(obj);
          group_command->add(CommandHandle(new ObjectAddCommand(layer, obj)));
        }
    }

  // Second pass to set the parents to the cloned objects
  for(Selection::iterator i = new_selection->begin(); i != new_selection->end(); ++i)
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

  execute(group_command);
  set_selection(new_selection);
}

void
WindstilleWidget::selection_reset_rotation()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      DecalObjectModel* decal = dynamic_cast<DecalObjectModel*>(i->get());

      if (decal)
        {
          //decal->set_angle(0.0f);
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&DecalObjectModel::set_angle, decal, decal->get_angle()),
                                                              boost::bind(&DecalObjectModel::set_angle, decal, 0.0f))));
        }
    }

  on_selection_change();
  execute(group_command);
}

void
WindstilleWidget::selection_object_properties()
{
  std::cout << "Display Dialog" << std::endl;
}

void
WindstilleWidget::selection_reset_scale()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

 for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      DecalObjectModel* decal = dynamic_cast<DecalObjectModel*>(i->get());
      if (decal)
        {
          //decal->set_scale(Vector2f(1.0f, 1.0f));
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&DecalObjectModel::set_scale, decal, decal->get_scale()),
                                                              boost::bind(&DecalObjectModel::set_scale, decal, Vector2f(1.0f, 1.0f)))));
        }
    }

 on_selection_change();
 execute(group_command);
}

void
WindstilleWidget::selection_delete()
{
  boost::shared_ptr<GroupCommand> group_cmd(new GroupCommand());
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      group_cmd->add(CommandHandle(new ObjectRemoveCommand(*sector_model, *i)));
    }

  execute(group_cmd);
  selection->clear();
  queue_draw();
}

bool
WindstilleWidget::scroll(GdkEventScroll* event_)
{
  if (event_->direction == GDK_SCROLL_UP)
    {
      //viewer->get_state().zoom(1.1f, Vector2i(event->x, event->y));
    }
  else if (event_->direction == GDK_SCROLL_DOWN)
    {
      //viewer->get_state().zoom(1.0f/1.1f, Vector2i(event->x, event->y));
    }
  return false;
}

bool
WindstilleWidget::mouse_down(GdkEventButton* event_)
{
  grab_focus();

  //std::cout << "Button Press: " << event_->x << ", " << event_->y << " - " << event_->button << std::endl;

  if (event_->button == 1)
    { // Tool
      EditorWindow::current()->get_current_tool()->mouse_down(event_, *this);
      return true;
    }
  else if (event_->button == 2)
    { // Scroll
      scroll_tool->mouse_down(event_, *this);
      return true;
    }
  else if (event_->button == 3)
    { // Context Menu
      EditorWindow::current()->get_current_tool()->mouse_right_down(event_, *this);
      return true;
    }
  else
    {
      return false;
    }
}

bool
WindstilleWidget::mouse_move(GdkEventMotion* event_)
{
  //std::cout << "Motion: " << event_->x << ", " << event_->y << std::endl;
  
  EditorWindow::current()->get_current_tool()->mouse_move(event_, *this);
  scroll_tool->mouse_move(event_, *this);
  
  return true;
}

bool
WindstilleWidget::mouse_up(GdkEventButton* event_)
{
  //std::cout << "Button Release: " << event_->x << ", " << event_->y << " - " << event_->button << std::endl;
  //viewer->on_mouse_button_up(Vector2i(event_->x, event_->y), event_->button);
  if (event_->button == 1)
    {
      EditorWindow::current()->get_current_tool()->mouse_up(event_, *this);
      queue_draw();
    }
  else if (event_->button == 2)
    {
      scroll_tool->mouse_up(event_, *this);
      queue_draw();
    }

  return false;
}

bool
WindstilleWidget::key_press(GdkEventKey* event_)
{
  //std::cout << event_->keyval << " keypress " << state.get_pos() << std::endl;

  switch(event_->keyval)
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
        selection_duplicate();
        break;

      case GDK_s:
        SurfaceManager::current()->save_all_as_png();
        break;

      case GDK_Delete:
        selection_delete();
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
WindstilleWidget::key_release(GdkEventKey* /*event_*/)
{ // /usr/include/gtk-2.0/gdk/gdkkeysyms.h
  //std::cout << "KeyRelease: " << (int)event_->keyval << std::endl;
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
      execute(CommandHandle(new ObjectAddCommand(sector_model->get_layer(path_), object)));
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

void
WindstilleWidget::set_selection(const SelectionHandle& selection_)
{
  selection = selection_;
  selection->signal_changed.connect(sigc::mem_fun(*this, &WindstilleWidget::on_selection_change));
  on_selection_change();
}

SelectionHandle
WindstilleWidget::get_selection() const
{
  return selection;
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
      return sector_model->get_layer(path_);  
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
WindstilleWidget::on_selection_change()
{
  control_points.clear();

  if (!selection->is_moving())
    {
      selection->add_control_points(control_points);
    }
  
  queue_draw();
}

ControlPointHandle
WindstilleWidget::get_control_point(const Vector2f& pos) const
{
  for(std::vector<ControlPointHandle>::const_iterator i = control_points.begin();  
      i != control_points.end(); ++i)
    {
      if ((*i)->get_bounding_box().is_inside(pos))
        return *i;
    }
  return ControlPointHandle();
}

void
WindstilleWidget::clear_control_points()
{
  control_points.clear();
}

void
WindstilleWidget::create_control_points()
{
  control_points.clear();
  selection->add_control_points(control_points);
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
