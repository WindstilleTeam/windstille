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

#include <gtkmm/toolbar.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/uimanager.h>
#include <boost/shared_ptr.hpp>

#include "editor/editor_window.hpp"
#include "editor/timeline_widget.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_keyframe_object.hpp"

AnimationWidget::AnimationWidget(EditorWindow& editor) :
  hadjustment(50, 0, 100),
  vadjustment(50, 0, 100),
  hruler(),
  scrolled(),
  treeview(),
  table(),
  vscroll(vadjustment),
  hscroll(hadjustment),
  m_timeline_widget(),
  m_timeline_layer_widget(),
  m_timeline()
{
  Glib::RefPtr<Gtk::UIManager>   ui_manager   = editor.get_ui_manager();
  Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("MenuAnimation",   "_Animation"));
  action_group->add(Gtk::Action::create("NewAnimation", Gtk::Stock::NEW),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_new));
  action_group->add(Gtk::Action::create("ExportAnimation", Gtk::Stock::SAVE_AS),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_export));
  action_group->add(Gtk::Action::create("DeleteAnimation", Gtk::Stock::DELETE),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_delete));

  action_group->add(Gtk::Action::create("NewLayerAnimation", Gtk::Stock::NEW),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_layer_new));

  action_group->add(Gtk::Action::create("FrameForwardAnimation", Gtk::Stock::GO_FORWARD),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_frame_forward));
  action_group->add(Gtk::Action::create("FrameBackwardAnimation", Gtk::Stock::GO_BACK),
                    sigc::mem_fun(editor, &EditorWindow::on_animation_frame_backward));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar  name='AnimationToolBar'>"
                                 "    <toolitem action='NewAnimation'/>"
                                 "    <toolitem action='ExportAnimation'/>"
                                 "    <toolitem action='DeleteAnimation'/>"
                                 "    <separator />"
                                 "    <toolitem action='NewLayerAnimation'/>"
                                 "    <separator />"
                                 "    <toolitem action='FrameBackwardAnimation'/>"
                                 "    <toolitem action='FrameForwardAnimation'/>"
                                 "    <separator />"
                                 "  </toolbar>"
                                 "</ui>");
  
  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/AnimationToolBar"));
  
  hruler.set_range(0, 100, 50, 100);

  table.attach(hruler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);

  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
  scrolled.add(treeview);

  m_timeline_layer_widget.set_size_request(140, -1);

  table.attach(m_timeline_layer_widget, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
  table.attach(m_timeline_widget, 1, 2, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  table.attach(hscroll, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);
  table.attach(vscroll, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);

  m_hbox.pack_start(toolbar, Gtk::PACK_EXPAND_WIDGET);

  //toolbar.set_icon_size(Gtk::ICON_SIZE_MENU);
  pack_start(m_hbox, Gtk::PACK_SHRINK);
  pack_start(table, Gtk::PACK_EXPAND_WIDGET);

  show_all();
}

void
AnimationWidget::set_timeline(TimelineHandle timeline)
{
  m_timeline = timeline;
  m_timeline_widget.set_timeline(timeline);
  m_timeline_layer_widget.set_timeline(timeline);
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

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  // create test timeline  
  boost::shared_ptr<Timeline> timeline(new Timeline());

  TimelineLayerHandle layer1 = timeline->add_layer("DoorLeft:pos");
  TimelineLayerHandle layer2 = timeline->add_layer("DoorRight:pos");
  TimelineLayerHandle layer3 = timeline->add_layer("AnimaitonLayer1");
  TimelineLayerHandle layer4 = timeline->add_layer("SoundLayer2");

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

  // create the window
  Gtk::Window win;
  win.set_title("Timeline Test");
  win.set_default_size(800, 400);

  AnimationWidget animation_widget();
  animation_widget.set_timeline(timeline);
  win.add(animation_widget);
  win.show_all();

  Gtk::Main::run(win);

  return 0;
}

#endif

/* EOF */
