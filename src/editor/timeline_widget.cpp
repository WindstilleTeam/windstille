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

#include "editor/timeline_widget.hpp"

#include <iostream>

#include "editor/timeline.hpp"

TimelineWidget::TimelineWidget() :
  m_timeline(),
  down_pos(),
  move_pos()
{
  signal_button_release_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_up));
  signal_button_press_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_down));
  signal_motion_notify_event().connect(sigc::mem_fun(this, &TimelineWidget::mouse_move));

  add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::KEY_PRESS_MASK      | Gdk::KEY_RELEASE_MASK | 
             Gdk::ENTER_NOTIFY_MASK   | Gdk::LEAVE_NOTIFY_MASK);
}

TimelineWidget::~TimelineWidget()
{
}

bool
TimelineWidget::mouse_down(GdkEventButton* ev)
{
  down_pos.x = static_cast<float>(ev->x);
  down_pos.y = static_cast<float>(ev->y);
  return false;
}

bool
TimelineWidget::mouse_up(GdkEventButton* ev)
{
  return false;
}

bool
TimelineWidget::mouse_move(GdkEventMotion* ev)
{
  move_pos.x = static_cast<float>(ev->x);
  move_pos.y = static_cast<float>(ev->y);
  queue_draw();
  return false;
}

bool
TimelineWidget::on_expose_event(GdkEventExpose* ev)
{
  if (Glib::RefPtr<Gdk::Window> window = get_window())
  {
    Gtk::Allocation allocation = get_allocation();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    std::cout << "on_expose_event: " << allocation.get_width() << "x" << allocation.get_height() << std::endl;

    if (1) // pixel perfect drawing
      cr->translate(0.5, 0.5);

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    cr->rectangle(ev->area.x,     ev->area.y,
                  ev->area.width, ev->area.height);
    cr->clip();

    cr->set_source_rgb(1,1,1);
    cr->rectangle(0, 0, allocation.get_width(), 32*4);
    cr->fill();

    cr->set_line_width(0.5);
    cr->set_source_rgb(0,0,0);
    // draw grid
    for(int y = 0; y < allocation.get_height(); y += 32)
    {
      cr->move_to(0, y);
      cr->line_to(allocation.get_width(), y);

      if (0)
        for(int x = 0; x < allocation.get_width(); x += 8)
        {
          cr->move_to(x, y - 3);
          cr->line_to(x, y + 3);
        }
    }
    cr->stroke();

    cr->set_source_rgba(0,0,0, 0.25);
    for(int x = 0; x < allocation.get_width(); x += 8)
    {
      if (x % 80)
      {
        cr->move_to(x, 0);
        cr->line_to(x, allocation.get_height());
      }
    }
    cr->stroke();

    cr->set_source_rgba(0,0,0, 0.75);
    for(int x = 0; x < allocation.get_width(); x += 8)
    {
      if (!(x % 80))
      {
        cr->move_to(x, 0);
        cr->line_to(x, allocation.get_height());
      }
    }
    cr->stroke();

    cr->set_line_width(1.0);

    // draw keyframe
    for(int x = 32; x < 128; x += 16)
    {
      cr->set_source_rgb(0.0, 0.5, 0.75);
      cr->rectangle(x, 32+4, 8, 24);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(x, 32+4, 8, 24);
      cr->stroke();
    }

    // print some text
    {
      std::string text = "snd:Hello World!";
      Cairo::TextExtents extents;

      cr->select_font_face ("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size(12);

      cr->get_text_extents(text, extents);

      std::cout << extents.x_bearing << " "
                << extents.y_bearing << " "
                << extents.width << " "
                << extents.height << " "
                << extents.x_advance << " "
                << extents.y_advance << " "
                << std::endl;

      double x_border = 16.0f;
      double y_border = 4.0f;
      cr->set_source_rgb(0.0, 0.5, 0.75);
      cr->rectangle(20  + extents.x_bearing - x_border, 
                    118 + extents.y_bearing - y_border,
                    extents.width  + 2*x_border,
                    extents.height + 2*y_border);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(20  + extents.x_bearing - x_border, 
                    118 + extents.y_bearing - y_border,
                    extents.width  + 2*x_border,
                    extents.height + 2*y_border);
      cr->stroke();

      cr->set_source_rgb(1,1,1);
      cr->move_to(20,118);
      cr->show_text(text);
    }

  std::cout << "down: " << down_pos << std::endl;
  std::cout << "move: " << move_pos << std::endl;

    // Select rectangle
    {
      cr->set_source_rgba(0,0,1,0.25);
      cr->rectangle(down_pos.x, down_pos.y,
                    move_pos.x - down_pos.x,
                    move_pos.y - down_pos.y);
      cr->fill();

      cr->set_source_rgb(0,0,1);
      cr->rectangle(down_pos.x, down_pos.y,
                    move_pos.x - down_pos.x,
                    move_pos.y - down_pos.y);
      cr->stroke();
    }
  }

  return true;
}

void
TimelineWidget::set_timeline(boost::shared_ptr<Timeline> timeline)
{
  m_timeline = timeline;
}

#ifdef __TEST__

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/table.h>
#include <gtkmm/ruler.h>
#include <gtkmm/scrollbar.h>

class TimelineLayerColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<Glib::ustring> m_name;

public:
  TimelineLayerColumns() :
    m_name()
  {
    add(m_name);
  }
};

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::Window win;
  win.set_title("Timeline Test");
  win.set_default_size(800, 400);

  TimelineLayerColumns columns;
  Gtk::HRuler hruler;
  Gtk::TreeView treeview;
  treeview.set_headers_visible(false);
  //treeview.set_fixed_height_mode(true);
  treeview.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_BOTH);
  treeview.remove_all_columns();
  treeview.append_column_editable("Name", columns.m_name);

  Glib::RefPtr<Gtk::ListStore> liststore = Gtk::ListStore::create(columns);

  for(int i = 0; i < 5; ++i)
  {
    Gtk::ListStore::iterator it = liststore->append();
    std::ostringstream str;
    str << "Hello World: " << i;
    (*it)[columns.m_name] = str.str();
  }

  treeview.set_model(liststore);
  treeview.set_size_request(200, -1);
  treeview.set_reorderable();

  Gtk::Table table;

  //table.set_size_request(800, 400);

  Gtk::Adjustment hadjustment(50, 0, 100);
  Gtk::Adjustment vadjustment(50, 0, 100);

  Gtk::VScrollbar vscroll(vadjustment);
  Gtk::HScrollbar hscroll(hadjustment);
   
  boost::shared_ptr<Timeline> timeline(new Timeline());

  timeline->add_layer("Layer1");
  timeline->add_layer("Layer2");

  TimelineWidget timeline_widget;
  timeline_widget.set_timeline(timeline);

  hruler.set_range(0, 100, 50, 100);

  table.attach(hruler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);

  table.attach(treeview, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
  table.attach(timeline_widget, 1, 2, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  table.attach(hscroll, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);
  table.attach(vscroll, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
  timeline_widget.show();

  win.add(table);
  win.show_all();

  Gtk::Main::run(win);

  return 0;
}

#endif

/* EOF */
