/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_TIMELINE_PANEL_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_TIMELINE_PANEL_HPP

#include <memory>

#include <QWidget>

class QTimer;

class QListWidget;
class QDoubleSpinBox;
class QSlider;
class QToolBar;

namespace windstille {

class Document;
class GLWidget;
class TimelineTrackWidget;
class TimelineObject;
typedef std::shared_ptr<TimelineObject> TimelineObjectHandle;

/** Bottom panel: timeline layers list, scrubber, keyframe actions. */
class TimelinePanel final : public QWidget
{
  Q_OBJECT

public:
  explicit TimelinePanel(QWidget* parent = nullptr);
  ~TimelinePanel() override;

  void set_document(Document* document);
  void set_gl_widget(GLWidget* widget) { m_gl_widget = widget; }

  float cursor_pos() const;

public slots:
  void rebuild();

private slots:
  void on_cursor_changed(double pos);
  void on_slider_changed(int value);
  void on_add_layer();
  void on_add_keyframe_pos();
  void on_add_keyframe_rot();
  void on_add_keyframe_scale();
  void on_apply();
  void on_play();
  void on_stop();
  void on_loop_toggled(bool checked);
  void on_zoom_in();
  void on_zoom_out();
  void on_play_tick();
  void on_delete_keyframe();

private:
  Document* m_document;
  GLWidget* m_gl_widget;
  QListWidget* m_layers;
  TimelineTrackWidget* m_track;
  QDoubleSpinBox* m_cursor;
  QSlider* m_slider;
  QTimer* m_play_timer;
  bool m_loop;
  bool m_updating;

  TimelinePanel(TimelinePanel const&) = delete;
  TimelinePanel& operator=(TimelinePanel const&) = delete;
};

/** Simple painted track showing cursor and keyframe markers. */
class TimelineTrackWidget final : public QWidget
{
  Q_OBJECT

public:
  explicit TimelineTrackWidget(QWidget* parent = nullptr);

  void set_document(Document* document);
  void set_cursor(float pos);
  float cursor() const { return m_cursor; }
  void zoom_in();
  void zoom_out();
  float duration() const { return m_duration; }
  TimelineObjectHandle selected_object() const { return m_selected; }
  void clear_selection() { m_selected.reset(); update(); }

signals:
  void cursor_scrubbed(float pos);
  void selection_changed();

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  float x_to_pos(int x) const;
  int pos_to_x(float pos) const;
  TimelineObjectHandle hit_test(int x, int y) const;

  Document* m_document;
  float m_cursor;
  float m_pixels_per_unit;
  float m_duration;
  TimelineObjectHandle m_selected;
  TimelineObjectHandle m_dragging;
  float m_drag_origin_pos;
  float m_drag_click_pos;
  bool m_scrubbing;
};

} // namespace windstille

#endif

/* EOF */
