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

#include "editor/animation_widget.hpp"

#include <boost/shared_ptr.hpp>

#include "editor/timeline_widget.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_keyframe_object.hpp"

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

AnimationWidget::AnimationWidget() :
  hadjustment(50, 0, 100),
  vadjustment(50, 0, 100),
  hruler(),
  treeview(),
  table(),
  vscroll(vadjustment),
  hscroll(hadjustment)
{
  TimelineLayerColumns columns;
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

  add(table);
}

/* EOF */
