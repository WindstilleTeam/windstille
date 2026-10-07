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

#include "editor_qt/property_panel.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "editor/decal_object_model.hpp"
#include "editor/document.hpp"
#include "editor/selection.hpp"
#include "editor_qt/gl_widget.hpp"

namespace windstille {

namespace {

QDoubleSpinBox* make_spin(QWidget* parent, double minv, double maxv, double step)
{
  auto* s = new QDoubleSpinBox(parent);
  s->setRange(minv, maxv);
  s->setSingleStep(step);
  s->setDecimals(3);
  return s;
}

} // namespace

PropertyPanel::PropertyPanel(QWidget* parent) :
  QWidget(parent),
  m_document(nullptr),
  m_gl_widget(nullptr),
  m_updating(false),
  m_name_label(new QLabel(tr("(no selection)"), this)),
  m_pos_x(make_spin(this, -1e6, 1e6, 1.0)),
  m_pos_y(make_spin(this, -1e6, 1e6, 1.0)),
  m_scale_x(make_spin(this, 0.01, 100.0, 0.05)),
  m_scale_y(make_spin(this, 0.01, 100.0, 0.05)),
  m_angle(make_spin(this, -360.0, 360.0, 1.0)),
  m_hflip(new QCheckBox(tr("Horizontal flip"), this)),
  m_vflip(new QCheckBox(tr("Vertical flip"), this))
{
  auto* form = new QFormLayout;
  form->addRow(tr("Object"), m_name_label);
  form->addRow(tr("X"), m_pos_x);
  form->addRow(tr("Y"), m_pos_y);
  form->addRow(tr("Scale X"), m_scale_x);
  form->addRow(tr("Scale Y"), m_scale_y);
  form->addRow(tr("Angle °"), m_angle);
  form->addRow(tr(""), m_hflip);
  form->addRow(tr(""), m_vflip);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);
  layout->addWidget(new QLabel(tr("Properties"), this));
  layout->addLayout(form);
  layout->addStretch(1);

  connect(m_pos_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &PropertyPanel::on_pos_x_changed);
  connect(m_pos_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &PropertyPanel::on_pos_y_changed);
  connect(m_scale_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &PropertyPanel::on_scale_x_changed);
  connect(m_scale_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &PropertyPanel::on_scale_y_changed);
  connect(m_angle, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &PropertyPanel::on_angle_changed);
  connect(m_hflip, &QCheckBox::stateChanged, this, &PropertyPanel::on_hflip_changed);
  connect(m_vflip, &QCheckBox::stateChanged, this, &PropertyPanel::on_vflip_changed);

  setMinimumWidth(180);
  setMaximumWidth(320);
  setEnabled(false);
}

PropertyPanel::~PropertyPanel() = default;

void
PropertyPanel::set_document(Document* document)
{
  m_document = document;
  if (m_document) {
    m_document->signal_on_change().connect([this]() {
      refresh_from_selection();
    });
  }
  refresh_from_selection();
}

void
PropertyPanel::refresh_from_selection()
{
  m_updating = true;
  if (!m_document || !m_document->get_selection() ||
      m_document->get_selection()->empty()) {
    m_name_label->setText(tr("(no selection)"));
    setEnabled(false);
    m_updating = false;
    return;
  }

  ObjectModelHandle obj = *m_document->get_selection()->begin();
  m_name_label->setText(QString::fromStdString(obj->get_name()));
  glm::vec2 const pos = obj->get_rel_pos();
  m_pos_x->setValue(pos.x);
  m_pos_y->setValue(pos.y);

  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (decal) {
    m_scale_x->setEnabled(true);
    m_scale_y->setEnabled(true);
    m_angle->setEnabled(true);
    m_hflip->setEnabled(true);
    m_vflip->setEnabled(true);
    m_scale_x->setValue(decal->get_scale().x);
    m_scale_y->setValue(decal->get_scale().y);
    m_angle->setValue(decal->get_angle() * 180.0 / 3.14159265358979323846);
    m_hflip->setChecked(decal->get_hflip());
    m_vflip->setChecked(decal->get_vflip());
  } else {
    m_scale_x->setEnabled(false);
    m_scale_y->setEnabled(false);
    m_angle->setEnabled(false);
    m_hflip->setEnabled(false);
    m_vflip->setEnabled(false);
  }

  setEnabled(true);
  m_updating = false;
}

void
PropertyPanel::on_pos_x_changed(double v)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  glm::vec2 pos = obj->get_rel_pos();
  pos.x = static_cast<float>(v);
  m_document->object_set_pos(obj, pos);
  if (m_gl_widget) m_gl_widget->update();
}

void
PropertyPanel::on_pos_y_changed(double v)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  glm::vec2 pos = obj->get_rel_pos();
  pos.y = static_cast<float>(v);
  m_document->object_set_pos(obj, pos);
  if (m_gl_widget) m_gl_widget->update();
}

void
PropertyPanel::on_scale_x_changed(double v)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  glm::vec2 s = decal->get_scale();
  s.x = static_cast<float>(v);
  decal->set_scale(s);
  m_document->signal_on_change()();
  if (m_gl_widget) {
    m_gl_widget->document().create_control_points();
    m_gl_widget->update();
  }
}

void
PropertyPanel::on_scale_y_changed(double v)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  glm::vec2 s = decal->get_scale();
  s.y = static_cast<float>(v);
  decal->set_scale(s);
  m_document->signal_on_change()();
  if (m_gl_widget) {
    m_gl_widget->document().create_control_points();
    m_gl_widget->update();
  }
}

void
PropertyPanel::on_angle_changed(double v)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  decal->set_angle(static_cast<float>(v * 3.14159265358979323846 / 180.0));
  m_document->signal_on_change()();
  if (m_gl_widget) {
    m_gl_widget->document().create_control_points();
    m_gl_widget->update();
  }
}


void
PropertyPanel::on_hflip_changed(int state)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  decal->set_hflip(state == Qt::Checked);
  m_document->signal_on_change()();
  if (m_gl_widget) m_gl_widget->update();
}

void
PropertyPanel::on_vflip_changed(int state)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  decal->set_vflip(state == Qt::Checked);
  m_document->signal_on_change()();
  if (m_gl_widget) m_gl_widget->update();
}

} // namespace windstille

/* EOF */
