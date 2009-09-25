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
#include <sstream>

#include "math/rect.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_keyframe_object.hpp"

TimelineWidget::TimelineWidget() :
  m_timeline(),
  m_selection(),
  is_mouse_down(false),
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
  if (ev->button == 1)
  {
    is_mouse_down = true;
    down_pos.x = static_cast<float>(ev->x);
    down_pos.y = static_cast<float>(ev->y);
  }
  return false;
}

bool
TimelineWidget::mouse_up(GdkEventButton* ev)
{
  if (ev->button == 1)
  {
    is_mouse_down = false;
    queue_draw();
  }
  return false;
}

bool
TimelineWidget::mouse_move(GdkEventMotion* ev)
{
  if (is_mouse_down)
  {
    move_pos.x = static_cast<float>(ev->x);
    move_pos.y = static_cast<float>(ev->y);
    queue_draw();
  }
  return false;
}

bool
TimelineWidget::on_expose_event(GdkEventExpose* ev)
{
  if (Glib::RefPtr<Gdk::Window> window = get_window())
  {
    Gtk::Allocation allocation = get_allocation();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //std::cout << "on_expose_event: " << allocation.get_width() << "x" << allocation.get_height() << std::endl;

    if (1) // pixel perfect drawing
      cr->translate(0.5, 0.5);

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    cr->rectangle(ev->area.x,     ev->area.y,
                  ev->area.width, ev->area.height);
    cr->clip();

    draw_timeline(cr);

    cr->set_line_width(0.5);
    cr->set_source_rgb(0,0,0);

    cr->set_line_width(1.0);

    draw_select_rectangle(cr);
  }

  return true;
}

void
TimelineWidget::draw_select_rectangle(Cairo::RefPtr<Cairo::Context> cr)
{
  // Select rectangle
  if (is_mouse_down)
  {
    //Rectf rect(down_pos, move_pos - down_pos);
    //rect.normalize();

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

void
TimelineWidget::draw_grid(Cairo::RefPtr<Cairo::Context> cr)
{
  Gtk::Allocation allocation = get_allocation();

  cr->save();
  
  int height = m_timeline->size() * 32;

  // draw vertical lines
  cr->set_source_rgb(0.875, 0.875, 0.875);
  for(int x = 0; x < allocation.get_width(); x += 8)
  {
    if (x % 80 == 0)
    {
      cr->rectangle(x, 0, 8, height);
    }
  }
  cr->fill();

  cr->set_line_width(1.0);

  // draw horizontal lines
  cr->set_source_rgb(0.5, 0.5, 0.5);
  for(int y = 0; y <= height; y += 32)
  {
    cr->move_to(0, y);
    cr->line_to(allocation.get_width(), y);
  }
  cr->stroke();

  // draw vertical lines
  cr->set_source_rgb(0.75, 0.75, 0.75);
  for(int x = 0; x < allocation.get_width(); x += 8)
  {
      cr->move_to(x, 0);
      cr->line_to(x, height);
  }
  cr->stroke();

  cr->restore();
}

void
TimelineWidget::draw_timeline(Cairo::RefPtr<Cairo::Context> cr)
{
  Gtk::Allocation allocation = get_allocation();

  cr->save();

  cr->set_source_rgb(1,1,1);
  cr->rectangle(0, 0, allocation.get_width(), 32 * m_timeline->size());
  cr->fill();
  
  draw_grid(cr);

  cr->save();
  for(Timeline::iterator i = m_timeline->begin(); i != m_timeline->end(); ++i)
  {
    draw_timeline_layer(cr, *i);
    cr->translate(0, 32);
  }
  cr->restore();

  cr->restore();
}

void
TimelineWidget::draw_timeline_layer(Cairo::RefPtr<Cairo::Context> cr,
                                    TimelineLayerHandle layer)
{
  cr->save();

  cr->set_line_width(1.0);

  for(TimelineLayer::iterator i = layer->begin(); i != layer->end(); ++i)
  {
    if (boost::shared_ptr<TimelineKeyframeObject> keyframe =
        boost::dynamic_pointer_cast<TimelineKeyframeObject>(*i))
    {
      cr->set_source_rgb(0.5, 0.75, 0.0);
      cr->rectangle(keyframe->get_pos()*8, 4, 8, 24);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(keyframe->get_pos()*8, 4, 8, 24);
      cr->stroke();     
    }
    else if (boost::shared_ptr<TimelineAnimObject> anim =
             boost::dynamic_pointer_cast<TimelineAnimObject>(*i))
    {
      Cairo::TextExtents extents;

      cr->select_font_face ("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size(12);

      cr->get_text_extents(anim->get_name(), extents);

      if (0)
        std::cout << extents.x_bearing << " "
                  << extents.y_bearing << " "
                  << extents.width << " "
                  << extents.height << " "
                  << extents.x_advance << " "
                  << extents.y_advance << " "
                  << std::endl;

      cr->set_source_rgb(0.0, 0.5, 0.75);
      cr->rectangle(anim->get_pos()*8,  4,
                    anim->get_width()*8, 24);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(anim->get_pos()*8, 4,
                    anim->get_width()*8, 24);
      cr->stroke();

      cr->set_source_rgb(1,1,1);
      cr->move_to((anim->get_pos() + anim->get_width()/2)*8 - extents.width/2, 
                  20);
      cr->show_text(anim->get_name());
    }
    else if (boost::shared_ptr<TimelineSoundObject> sound =
             boost::dynamic_pointer_cast<TimelineSoundObject>(*i))
    {
      Cairo::TextExtents extents;

      cr->select_font_face ("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size(12);

      cr->get_text_extents(sound->get_name(), extents);

      if (0)
        std::cout << extents.x_bearing << " "
                  << extents.y_bearing << " "
                  << extents.width << " "
                  << extents.height << " "
                  << extents.x_advance << " "
                  << extents.y_advance << " "
                  << std::endl;

      cr->set_source_rgb(0.75, 0.0, 0.5);
      cr->rectangle(sound->get_pos()*8,  4,
                    sound->get_width()*8, 24);
      cr->fill();

      cr->set_source_rgb(0, 0, 0);
      cr->rectangle(sound->get_pos()*8, 4,
                    sound->get_width()*8, 24);
      cr->stroke();

      cr->set_source_rgb(1,1,1);
      cr->move_to((sound->get_pos() + sound->get_width()/2)*8 - extents.width/2, 
                  20);
      cr->show_text(sound->get_name());
    }
  }
  
  cr->restore();
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

  TimelineLayerHandle layer1 = timeline->add_layer("Layer1");
  TimelineLayerHandle layer2 = timeline->add_layer("Layer2");
  TimelineLayerHandle layer3 = timeline->add_layer("Layer3");
  TimelineLayerHandle layer4 = timeline->add_layer("Layer4");

  layer1->add_object(TimelineObjectHandle(new TimelineAnimObject(20.0f, 30.0f, "anim1.anim")));
  layer1->add_object(TimelineObjectHandle(new TimelineAnimObject(60.0f, 30.0f, "anim2.anim")));

  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(8.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(10.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(11.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(15.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(20.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(35.0f)));
  layer2->add_object(TimelineObjectHandle(new TimelineKeyframeObject(40.0f)));

  layer3->add_object(TimelineObjectHandle(new TimelineSoundObject(10.0f, 10.0f, "sound1.wav")));
  layer3->add_object(TimelineObjectHandle(new TimelineSoundObject(30.0f, 40.0f, "sound.wav")));

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
