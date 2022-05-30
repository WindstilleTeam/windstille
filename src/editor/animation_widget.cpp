/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <glibmm/main.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/uimanager.h>
#include <memory>

#include "editor/editor_window.hpp"
#include "editor/windstille_widget.hpp"
#include "editor/timeline_widget.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_anim_object.hpp"
#include "editor/timeline_sound_object.hpp"
#include "editor/timeline_keyframe_object.hpp"

namespace windstille {

AnimationWidget::AnimationWidget(EditorWindow& editor) :
  m_editor(editor),
  hadjustment(Gtk::Adjustment::create(50, 0, 100)),
  vadjustment(Gtk::Adjustment::create(50, 0, 100)),
  m_hbox(),
  //hruler(),
  scrolled(),
  treeview(),
  table(),
  vscroll(),
  hscroll(),
  m_timeline_widget(m_editor),
  m_timeline_layer_widget(),
  m_timeline(),
  m_timeout_connection(),
  m_playing(false),
  m_anim_pos(0.0f)
{
  vscroll.set_adjustment(vadjustment);
  hscroll.set_adjustment(hadjustment);

  Glib::RefPtr<Gtk::UIManager>   ui_manager   = m_editor.get_ui_manager();
  Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create("AnimationWidget");

  action_group->add(Gtk::Action::create("MenuAnimation",   "_Animation"));
  action_group->add(Gtk::Action::create("NewAnimation", Gtk::Stock::NEW),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_new));
  action_group->add(Gtk::Action::create("ExportAnimation", Gtk::Stock::SAVE_AS),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_export));
  action_group->add(Gtk::Action::create("DeleteAnimation", Gtk::Stock::DELETE),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_delete));

  action_group->add(Gtk::Action::create("NewLayerAnimation", Gtk::Stock::NEW),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_layer_new));

  action_group->add(Gtk::Action::create("FrameForwardAnimation", Gtk::Stock::GO_FORWARD),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_frame_forward));
  action_group->add(Gtk::Action::create("FrameBackwardAnimation", Gtk::Stock::GO_BACK),
                    sigc::mem_fun(m_editor, &EditorWindow::on_animation_frame_backward));

  action_group->add(Gtk::Action::create("AnimationZoomIn",  Gtk::Stock::ZOOM_IN),
                    sigc::mem_fun(m_timeline_widget, &TimelineWidget::zoom_in));
  action_group->add(Gtk::Action::create("AnimationZoomOut", Gtk::Stock::ZOOM_OUT),
                    sigc::mem_fun(m_timeline_widget, &TimelineWidget::zoom_out));

  action_group->add(Gtk::Action::create("AnimationForward", Gtk::Stock::MEDIA_FORWARD));
  action_group->add(Gtk::Action::create("AnimationNext", Gtk::Stock::MEDIA_NEXT));
  action_group->add(Gtk::Action::create("AnimationPrevious", Gtk::Stock::MEDIA_PREVIOUS));
  action_group->add(Gtk::Action::create("AnimationRewind", Gtk::Stock::MEDIA_REWIND));
  action_group->add(Gtk::Action::create("AnimationPlay", Gtk::Stock::MEDIA_PLAY),
                    sigc::mem_fun(*this, &AnimationWidget::on_play));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar  name='AnimationToolBar'>"
                                 "    <toolitem action='NewAnimation'/>"
                                 "    <toolitem action='ExportAnimation'/>"
                                 "    <toolitem action='DeleteAnimation'/>"
                                 "    <separator />"
                                 "    <toolitem action='NewLayerAnimation'/>"
                                 "    <separator />"
                                 "    <toolitem action='AnimationZoomIn'/>"
                                 "    <toolitem action='AnimationZoomOut'/>"
                                 "    <separator />"
                                 "    <toolitem action='FrameBackwardAnimation'/>"
                                 "    <toolitem action='FrameForwardAnimation'/>"
                                 "    <separator />"
                                 "    <toolitem action='AnimationPlay'/>"
                                 "    <separator />"
                                 "    <toolitem action='AnimationPrevious'/>"
                                 "    <toolitem action='AnimationNext'/>"
                                 "    <separator />"
                                 "    <toolitem action='AnimationRewind'/>"
                                 "    <toolitem action='AnimationForward'/>"
                                 "  </toolbar>"
                                 "</ui>");

  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/AnimationToolBar"));

  //hruler.set_range(0, 100, 50, 100);

  //table.attach(hruler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);

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
AnimationWidget::on_play()
{
  if (!m_playing)
  {
    std::cout << "Play" << std::endl;
    m_playing = true;
    m_anim_pos = 0.0f;
    m_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimationWidget::on_timeout),
                                                          50,
                                                          Glib::PRIORITY_DEFAULT);
  }
  else
  {
    std::cout << "Stop" << std::endl;
    m_playing = false;
    m_timeout_connection.disconnect();
  }
}

bool
AnimationWidget::on_timeout()
{
  m_anim_pos += 0.5f;
  m_timeline->apply(m_anim_pos);
  if (WindstilleWidget* wst = m_editor.get_windstille_widget())
    wst->queue_draw();
  return true;
}

void
AnimationWidget::set_timeline(TimelineHandle timeline)
{
  m_timeline = timeline;
  m_timeline_widget.set_timeline(timeline);
  m_timeline_layer_widget.set_timeline(timeline);
}

void
AnimationWidget::on_export()
{

}

} // namespace windstille

/* EOF */
