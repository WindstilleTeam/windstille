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

class AnimationWidget : public Gtk::VBox
{
private:
  Gtk::Adjustment hadjustment;
  Gtk::Adjustment vadjustment;

  Gtk::HRuler     hruler;
  Gtk::TreeView   treeview;
  Gtk::Table      table;
  Gtk::VScrollbar vscroll;
  Gtk::HScrollbar hscroll;

public:
  AnimationWidget();

private:
  AnimationWidget(const AnimationWidget&);
  AnimationWidget& operator=(const AnimationWidget&);
};

#endif

/* EOF */
