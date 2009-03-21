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

#ifndef HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_WINDSTILLE_WIDGET_HPP

#include <memory>
#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <iostream>
#include <gtkglmm.h>
#include <gdkmm/dragcontext.h>
#include <gtkmm/gl/widget.h>

#include "display/graphic_context_state.hpp"
#include "display/scene_context.hpp"
#include "math/vector2f.hpp"

/** OpenGL drawing area into which the Windstille game will be
    embedded */
class WindstilleWidget 
  : public Gtk::DrawingArea,
    public Gtk::GL::Widget<WindstilleWidget>
{
private:
  GraphicContextState   state;
  std::vector<Vector2f> objects;
  std::auto_ptr<SceneContext> sc;
  
public:
  WindstilleWidget(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                   const Glib::RefPtr<const Gdk::GL::Context>& share_list);
  virtual ~WindstilleWidget();

  virtual void on_realize();
  virtual bool on_timeout();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);

  virtual bool mouse_move(GdkEventMotion* event);
  virtual bool mouse_down (GdkEventButton* event);
  virtual bool mouse_up(GdkEventButton* event);
  virtual bool scroll(GdkEventScroll* event);

  virtual bool key_press(GdkEventKey* event);
  virtual bool key_release(GdkEventKey* event);

  // Drag&Drop
  virtual void on_drag_finish(const Glib::RefPtr<Gdk::DragContext>& context);
  virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>&, int x, int y,
                                     const Gtk::SelectionData& data, guint info, guint time);
  virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);

private:
  WindstilleWidget (const WindstilleWidget&);
  WindstilleWidget& operator= (const WindstilleWidget&);
};

#endif

/* EOF */
