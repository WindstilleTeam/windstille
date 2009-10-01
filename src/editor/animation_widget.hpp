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

#ifndef HEADER_WINDSTILLE_ANIMATION_WIDGET_HPP
#define HEADER_WINDSTILLE_ANIMATION_WIDGET_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/main.h>
#include <gtkmm/ruler.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/table.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>

#include "editor/timeline_handles.hpp"
#include "editor/timeline_widget.hpp"
#include "editor/timeline_layer_widget.hpp"

class EditorWindow;

class AnimationWidget : public Gtk::VBox
{
private:
  Gtk::Adjustment hadjustment;
  Gtk::Adjustment vadjustment;

  Gtk::HBox       m_hbox;
  Gtk::HRuler     hruler;
  Gtk::ScrolledWindow scrolled;
  Gtk::TreeView   treeview;
  Gtk::Table      table;
  Gtk::VScrollbar vscroll;
  Gtk::HScrollbar hscroll;
  TimelineWidget  m_timeline_widget;
  TimelineLayerWidget  m_timeline_layer_widget;

  TimelineHandle m_timeline;
  sigc::connection m_timeout_connection;
  bool m_playing;
  float m_anim_pos;

public:
  AnimationWidget(EditorWindow& editor);

  void set_timeline(TimelineHandle timeline);
  TimelineWidget& get_timeline_widget() { return m_timeline_widget; }
  void on_play();
  void on_export();
  bool on_timeout();
  
private:
  AnimationWidget(const AnimationWidget&);
  AnimationWidget& operator=(const AnimationWidget&);
};

#endif

/* EOF */
