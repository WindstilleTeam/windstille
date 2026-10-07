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

#include <QWidget>

class QListWidget;
class QDoubleSpinBox;
class QSlider;
class QToolBar;

namespace windstille {

class Document;
class GLWidget;
class TimelineTrackWidget;

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

private:
  Document* m_document;
  GLWidget* m_gl_widget;
  QListWidget* m_layers;
  TimelineTrackWidget* m_track;
  QDoubleSpinBox* m_cursor;
  QSlider* m_slider;
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

signals:
  void cursor_scrubbed(float pos);

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private:
  float x_to_pos(int x) const;
  int pos_to_x(float pos) const;

  Document* m_document;
  float m_cursor;
  float m_pixels_per_unit;
  float m_duration;
};

} // namespace windstille

#endif

/* EOF */
