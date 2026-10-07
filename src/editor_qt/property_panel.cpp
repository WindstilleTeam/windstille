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
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <surf/color.hpp>

#include "editor/decal_object_model.hpp"
#include "editor/document.hpp"
#include "editor/select_mask.hpp"
#include "editor/selection.hpp"
#include "editor/sector_model.hpp"
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
  m_ambient_btn(new QPushButton(this)),
  m_name_label(new QLabel(tr("(no selection)"), this)),
  m_pos_x(make_spin(this, -1e6, 1e6, 1.0)),
  m_pos_y(make_spin(this, -1e6, 1e6, 1.0)),
  m_scale_x(make_spin(this, 0.01, 100.0, 0.05)),
  m_scale_y(make_spin(this, 0.01, 100.0, 0.05)),
  m_angle(make_spin(this, -360.0, 360.0, 1.0)),
  m_hflip(new QCheckBox(tr("Horizontal flip"), this)),
  m_vflip(new QCheckBox(tr("Vertical flip"), this)),
  m_map_type(new QComboBox(this)),
  m_mask_bits{},
  m_object_group(nullptr)
{
  m_map_type->addItem(tr("Color map"), static_cast<int>(DecalObjectModel::COLORMAP));
  m_map_type->addItem(tr("Light map"), static_cast<int>(DecalObjectModel::LIGHTMAP));
  m_map_type->addItem(tr("Highlight map"), static_cast<int>(DecalObjectModel::HIGHLIGHTMAP));

  auto* sector_form = new QFormLayout;
  sector_form->addRow(tr("Ambient"), m_ambient_btn);

  auto* object_form = new QFormLayout;
  object_form->addRow(tr("Object"), m_name_label);
  object_form->addRow(tr("X"), m_pos_x);
  object_form->addRow(tr("Y"), m_pos_y);
  object_form->addRow(tr("Scale X"), m_scale_x);
  object_form->addRow(tr("Scale Y"), m_scale_y);
  object_form->addRow(tr("Angle °"), m_angle);
  object_form->addRow(tr(""), m_hflip);
  object_form->addRow(tr(""), m_vflip);
  object_form->addRow(tr("Map"), m_map_type);

  auto* mask_row = new QHBoxLayout;
  mask_row->setSpacing(2);
  for (int i = 0; i < 16; ++i) {
    m_mask_bits[static_cast<size_t>(i)] = new QCheckBox(QString::number(i), this);
    m_mask_bits[static_cast<size_t>(i)]->setToolTip(tr("Select mask bit %1").arg(i));
    mask_row->addWidget(m_mask_bits[static_cast<size_t>(i)]);
    connect(m_mask_bits[static_cast<size_t>(i)], &QCheckBox::checkStateChanged,
            this, &PropertyPanel::on_select_mask_changed);
  }
  object_form->addRow(tr("Mask"), mask_row);

  m_object_group = new QGroupBox(tr("Selection"), this);
  m_object_group->setLayout(object_form);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);
  layout->addWidget(new QLabel(tr("Sector"), this));
  layout->addLayout(sector_form);
  layout->addWidget(m_object_group);
  layout->addStretch(1);

  connect(m_ambient_btn, &QPushButton::clicked, this, &PropertyPanel::on_ambient_clicked);
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
  connect(m_hflip, &QCheckBox::checkStateChanged, this, &PropertyPanel::on_hflip_changed);
  connect(m_vflip, &QCheckBox::checkStateChanged, this, &PropertyPanel::on_vflip_changed);
  connect(m_map_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &PropertyPanel::on_map_type_changed);

  setMinimumWidth(200);
  setMaximumWidth(360);
  set_object_widgets_enabled(false);
  update_ambient_button();
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
PropertyPanel::set_object_widgets_enabled(bool on)
{
  m_object_group->setEnabled(on);
}

void
PropertyPanel::update_ambient_button()
{
  if (!m_document) {
    m_ambient_btn->setText(tr("(none)"));
    m_ambient_btn->setEnabled(false);
    return;
  }
  m_ambient_btn->setEnabled(true);
  surf::Color const c = m_document->get_sector_model().get_ambient_color();
  QColor qc;
  qc.setRgbF(c.r, c.g, c.b, c.a);
  m_ambient_btn->setText(qc.name(QColor::HexRgb));
  QPalette pal = m_ambient_btn->palette();
  pal.setColor(QPalette::Button, qc);
  m_ambient_btn->setPalette(pal);
  m_ambient_btn->setAutoFillBackground(true);
}

void
PropertyPanel::refresh_from_selection()
{
  m_updating = true;
  update_ambient_button();

  if (!m_document || !m_document->get_selection() ||
      m_document->get_selection()->empty()) {
    m_name_label->setText(tr("(no selection)"));
    set_object_widgets_enabled(false);
    m_updating = false;
    return;
  }

  ObjectModelHandle obj = *m_document->get_selection()->begin();
  m_name_label->setText(QString::fromStdString(obj->get_name()));
  glm::vec2 const pos = obj->get_rel_pos();
  m_pos_x->setValue(pos.x);
  m_pos_y->setValue(pos.y);

  SelectMask const mask = obj->get_select_mask();
  for (int i = 0; i < 16; ++i) {
    m_mask_bits[static_cast<size_t>(i)]->setChecked(mask.get(static_cast<unsigned>(i)));
  }

  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (decal) {
    m_scale_x->setEnabled(true);
    m_scale_y->setEnabled(true);
    m_angle->setEnabled(true);
    m_hflip->setEnabled(true);
    m_vflip->setEnabled(true);
    m_map_type->setEnabled(true);
    m_scale_x->setValue(decal->get_scale().x);
    m_scale_y->setValue(decal->get_scale().y);
    m_angle->setValue(decal->get_angle() * 180.0 / 3.14159265358979323846);
    m_hflip->setChecked(decal->get_hflip());
    m_vflip->setChecked(decal->get_vflip());
    m_map_type->setCurrentIndex(static_cast<int>(decal->get_map_type()));
  } else {
    m_scale_x->setEnabled(false);
    m_scale_y->setEnabled(false);
    m_angle->setEnabled(false);
    m_hflip->setEnabled(false);
    m_vflip->setEnabled(false);
    m_map_type->setEnabled(false);
  }

  set_object_widgets_enabled(true);
  m_updating = false;
}

void
PropertyPanel::on_ambient_clicked()
{
  if (!m_document) {
    return;
  }
  surf::Color const cur = m_document->get_sector_model().get_ambient_color();
  QColor initial;
  initial.setRgbF(cur.r, cur.g, cur.b, cur.a);
  QColor const chosen = QColorDialog::getColor(
    initial, this, tr("Sector ambient color"), QColorDialog::ShowAlphaChannel);
  if (!chosen.isValid()) {
    return;
  }
  m_document->get_sector_model().set_ambient_color(surf::Color(
    static_cast<float>(chosen.redF()),
    static_cast<float>(chosen.greenF()),
    static_cast<float>(chosen.blueF()),
    static_cast<float>(chosen.alphaF())));
  m_document->signal_on_change()();
  update_ambient_button();
  if (m_gl_widget) {
    m_gl_widget->update();
  }
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
PropertyPanel::on_hflip_changed(Qt::CheckState state)
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
PropertyPanel::on_vflip_changed(Qt::CheckState state)
{
  if (m_updating || !m_document) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  decal->set_vflip(state == Qt::Checked);
  m_document->signal_on_change()();
  if (m_gl_widget) m_gl_widget->update();
}

void
PropertyPanel::on_map_type_changed(int index)
{
  if (m_updating || !m_document || index < 0) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  auto* decal = dynamic_cast<DecalObjectModel*>(obj.get());
  if (!decal) return;
  decal->set_map_type(static_cast<DecalObjectModel::MapType>(index));
  m_document->signal_on_change()();
  if (m_gl_widget) m_gl_widget->update();
}

void
PropertyPanel::on_select_mask_changed()
{
  if (m_updating || !m_document) return;
  if (!m_document->get_selection() || m_document->get_selection()->empty()) return;
  ObjectModelHandle obj = *m_document->get_selection()->begin();
  SelectMask mask(0);
  for (int i = 0; i < 16; ++i) {
    mask.set(static_cast<unsigned>(i),
             m_mask_bits[static_cast<size_t>(i)]->isChecked());
  }
  obj->set_select_mask(mask);
  m_document->signal_on_change()();
  if (m_gl_widget) m_gl_widget->update();
}

} // namespace windstille

/* EOF */
