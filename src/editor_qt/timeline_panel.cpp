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

#include "editor_qt/timeline_panel.hpp"

#include <algorithm>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
#include <QTimer>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>

#include "editor/document.hpp"
#include "editor/selection.hpp"
#include "editor/sector_model.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_layer.hpp"
#include "editor/timeline_object.hpp"
#include "editor/timeline_properties.hpp"
#include "editor_qt/gl_widget.hpp"

namespace windstille {

// --- TimelineTrackWidget ---------------------------------------------------

TimelineTrackWidget::TimelineTrackWidget(QWidget* parent) :
  QWidget(parent),
  m_document(nullptr),
  m_cursor(0.0f),
  m_pixels_per_unit(40.0f),
  m_duration(20.0f),
  m_selected(),
  m_dragging(),
  m_drag_origin_pos(0.0f),
  m_drag_click_pos(0.0f),
  m_scrubbing(false)
{
  setMinimumHeight(72);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFocusPolicy(Qt::ClickFocus);
}

void
TimelineTrackWidget::set_document(Document* document)
{
  m_document = document;
  update();
}

void
TimelineTrackWidget::set_cursor(float pos)
{
  m_cursor = pos;
  update();
}

void
TimelineTrackWidget::zoom_in()
{
  m_pixels_per_unit = std::min(200.0f, m_pixels_per_unit * 1.25f);
  update();
}

void
TimelineTrackWidget::zoom_out()
{
  m_pixels_per_unit = std::max(8.0f, m_pixels_per_unit / 1.25f);
  update();
}

float
TimelineTrackWidget::x_to_pos(int x) const
{
  return static_cast<float>(x) / m_pixels_per_unit;
}

int
TimelineTrackWidget::pos_to_x(float pos) const
{
  return static_cast<int>(pos * m_pixels_per_unit);
}

void
TimelineTrackWidget::paintEvent(QPaintEvent* /*event*/)
{
  QPainter p(this);
  p.fillRect(rect(), QColor(40, 42, 48));

  // Grid
  p.setPen(QColor(60, 62, 70));
  int const w = width();
  int const h = height();
  for (float t = 0.0f; t <= m_duration + 0.01f; t += 1.0f) {
    int const x = pos_to_x(t);
    p.drawLine(x, 0, x, h);
  }

  // Keyframe markers from all layers
  if (m_document) {
    TimelineHandle timeline = m_document->get_sector_model().get_timeline();
    if (timeline) {
      int row = 0;
      for (auto it = timeline->begin(); it != timeline->end(); ++it, ++row) {
        TimelineLayerHandle layer = *it;
        int const y = 8 + (row % 4) * 14;
        for (auto oit = layer->begin(); oit != layer->end(); ++oit) {
          int const x = pos_to_x((*oit)->get_pos());
          bool const sel = (m_selected && *oit == m_selected);
          p.setBrush(sel ? QColor(255, 120, 60) : QColor(220, 180, 60));
          p.setPen(sel ? QPen(Qt::white, 1) : Qt::NoPen);
          p.drawEllipse(QPoint(x, y), sel ? 6 : 5, sel ? 6 : 5);
        }
      }
    }
  }

  // Cursor
  int const cx = pos_to_x(m_cursor);
  p.setPen(QPen(QColor(80, 180, 255), 2));
  p.drawLine(cx, 0, cx, h);
  p.setBrush(QColor(80, 180, 255));
  p.setPen(Qt::NoPen);
  QPolygon tri;
  tri << QPoint(cx - 6, 0) << QPoint(cx + 6, 0) << QPoint(cx, 10);
  p.drawPolygon(tri);
}

TimelineObjectHandle
TimelineTrackWidget::hit_test(int x, int y) const
{
  if (!m_document) {
    return {};
  }
  TimelineHandle timeline = m_document->get_sector_model().get_timeline();
  if (!timeline) {
    return {};
  }
  int row = 0;
  for (auto it = timeline->begin(); it != timeline->end(); ++it, ++row) {
    TimelineLayerHandle layer = *it;
    int const ky = 8 + (row % 4) * 14;
    for (auto oit = layer->begin(); oit != layer->end(); ++oit) {
      int const kx = pos_to_x((*oit)->get_pos());
      int const dx = x - kx;
      int const dy = y - ky;
      if (dx * dx + dy * dy <= 8 * 8) {
        return *oit;
      }
    }
  }
  return {};
}

void
TimelineTrackWidget::mousePressEvent(QMouseEvent* event)
{
  int const mx = static_cast<int>(event->position().x());
  int const my = static_cast<int>(event->position().y());
  TimelineObjectHandle hit = hit_test(mx, my);
  if (hit) {
    m_selected = hit;
    m_dragging = hit;
    m_drag_origin_pos = hit->get_pos();
    m_drag_click_pos = x_to_pos(mx);
    m_scrubbing = false;
    emit selection_changed();
    update();
    return;
  }
  m_selected.reset();
  emit selection_changed();
  float const pos = std::max(0.0f, x_to_pos(mx));
  m_cursor = pos;
  m_scrubbing = true;
  update();
  emit cursor_scrubbed(pos);
}

void
TimelineTrackWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }
  int const mx = static_cast<int>(event->position().x());
  if (m_dragging) {
    float const delta = x_to_pos(mx) - m_drag_click_pos;
    float const neu = std::max(0.0f, m_drag_origin_pos + delta);
    m_dragging->set_pos(neu);
    update();
    return;
  }
  if (m_scrubbing) {
    float const pos = std::max(0.0f, x_to_pos(mx));
    m_cursor = pos;
    update();
    emit cursor_scrubbed(pos);
  }
}

void
TimelineTrackWidget::mouseReleaseEvent(QMouseEvent* /*event*/)
{
  if (m_dragging && m_document) {
    float const neu = m_dragging->get_pos();
    if (neu != m_drag_origin_pos) {
      // Reset to origin so the command records the correct old position.
      m_dragging->set_pos(m_drag_origin_pos);
      m_document->timeline_set_object_pos(m_dragging, neu);
    }
  }
  m_dragging.reset();
  m_scrubbing = false;
  update();
}

// --- TimelinePanel ---------------------------------------------------------

TimelinePanel::TimelinePanel(QWidget* parent) :
  QWidget(parent),
  m_document(nullptr),
  m_gl_widget(nullptr),
  m_layers(new QListWidget(this)),
  m_track(new TimelineTrackWidget(this)),
  m_cursor(new QDoubleSpinBox(this)),
  m_slider(new QSlider(Qt::Horizontal, this)),
  m_play_timer(new QTimer(this)),
  m_loop(false),
  m_updating(false)
{
  m_cursor->setRange(0.0, 9999.0);
  m_cursor->setDecimals(2);
  m_cursor->setSingleStep(0.1);
  m_slider->setRange(0, 2000);
  m_slider->setValue(0);

  auto* toolbar = new QToolBar(this);
  toolbar->addAction(tr("Play"), this, &TimelinePanel::on_play);
  toolbar->addAction(tr("Stop"), this, &TimelinePanel::on_stop);
  QAction* act_loop = toolbar->addAction(tr("Loop"));
  act_loop->setCheckable(true);
  connect(act_loop, &QAction::toggled, this, &TimelinePanel::on_loop_toggled);
  toolbar->addSeparator();
  toolbar->addAction(tr("Zoom+"), this, &TimelinePanel::on_zoom_in);
  toolbar->addAction(tr("Zoom-"), this, &TimelinePanel::on_zoom_out);
  toolbar->addSeparator();
  toolbar->addAction(tr("Add Layer"), this, &TimelinePanel::on_add_layer);
  toolbar->addSeparator();
  toolbar->addAction(tr("KF Pos"), this, &TimelinePanel::on_add_keyframe_pos);
  toolbar->addAction(tr("KF Rot"), this, &TimelinePanel::on_add_keyframe_rot);
  toolbar->addAction(tr("KF Scale"), this, &TimelinePanel::on_add_keyframe_scale);
  toolbar->addSeparator();
  toolbar->addAction(tr("Apply"), this, &TimelinePanel::on_apply);
  toolbar->addAction(tr("Del KF"), this, &TimelinePanel::on_delete_keyframe);

  auto* cursor_row = new QHBoxLayout;
  cursor_row->addWidget(new QLabel(tr("Time"), this));
  cursor_row->addWidget(m_cursor);
  cursor_row->addWidget(m_slider, 1);

  auto* right = new QVBoxLayout;
  right->addWidget(m_track);
  right->addLayout(cursor_row);

  auto* body = new QHBoxLayout;
  m_layers->setMaximumWidth(180);
  m_layers->setMinimumWidth(120);
  body->addWidget(m_layers);
  body->addLayout(right, 1);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(2);
  layout->addWidget(toolbar);
  layout->addLayout(body);

  connect(m_cursor, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &TimelinePanel::on_cursor_changed);
  connect(m_slider, &QSlider::valueChanged, this, &TimelinePanel::on_slider_changed);
  connect(m_track, &TimelineTrackWidget::cursor_scrubbed, this, [this](float pos) {
    m_updating = true;
    m_cursor->setValue(pos);
    m_slider->setValue(static_cast<int>(pos * 100.0f));
    m_updating = false;
    on_apply();
  });
  m_play_timer->setInterval(33); // ~30 fps
  connect(m_play_timer, &QTimer::timeout, this, &TimelinePanel::on_play_tick);

  setMinimumHeight(140);
  setMaximumHeight(280);
  setFocusPolicy(Qt::StrongFocus);
}

TimelinePanel::~TimelinePanel() = default;

float
TimelinePanel::cursor_pos() const
{
  return static_cast<float>(m_cursor->value());
}

void
TimelinePanel::set_document(Document* document)
{
  m_document = document;
  m_track->set_document(document);
  rebuild();
}

void
TimelinePanel::rebuild()
{
  m_layers->clear();
  if (!m_document) {
    m_track->update();
    return;
  }
  TimelineHandle timeline = m_document->get_sector_model().get_timeline();
  if (!timeline) {
    return;
  }
  for (auto it = timeline->begin(); it != timeline->end(); ++it) {
    m_layers->addItem(QString::fromStdString((*it)->get_name()));
  }
  m_track->update();
}

void
TimelinePanel::on_cursor_changed(double pos)
{
  if (m_updating) return;
  m_updating = true;
  m_slider->setValue(static_cast<int>(pos * 100.0));
  m_track->set_cursor(static_cast<float>(pos));
  m_updating = false;
  on_apply();
}

void
TimelinePanel::on_slider_changed(int value)
{
  if (m_updating) return;
  m_updating = true;
  double const pos = value / 100.0;
  m_cursor->setValue(pos);
  m_track->set_cursor(static_cast<float>(pos));
  m_updating = false;
  on_apply();
}

void
TimelinePanel::on_add_layer()
{
  if (!m_document) return;
  m_document->timeline_add_layer("Layer");
  rebuild();
}

void
TimelinePanel::on_add_keyframe_pos()
{
  if (!m_document) return;
  SelectionHandle sel = m_document->get_selection();
  if (!sel || sel->empty()) return;
  m_document->timeline_add_keyframe(*sel->begin(), kPosition, cursor_pos());
  rebuild();
}

void
TimelinePanel::on_add_keyframe_rot()
{
  if (!m_document) return;
  SelectionHandle sel = m_document->get_selection();
  if (!sel || sel->empty()) return;
  m_document->timeline_add_keyframe(*sel->begin(), kRotation, cursor_pos());
  rebuild();
}

void
TimelinePanel::on_add_keyframe_scale()
{
  if (!m_document) return;
  SelectionHandle sel = m_document->get_selection();
  if (!sel || sel->empty()) return;
  m_document->timeline_add_keyframe(*sel->begin(), kScale, cursor_pos());
  rebuild();
}

void
TimelinePanel::on_apply()
{
  if (!m_document) return;
  TimelineHandle timeline = m_document->get_sector_model().get_timeline();
  if (!timeline) return;
  timeline->apply(cursor_pos());
  if (m_gl_widget) {
    m_gl_widget->update();
  }
}

} // namespace windstille

/* EOF */
