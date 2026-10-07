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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_PROPERTY_PANEL_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_PROPERTY_PANEL_HPP

#include <QWidget>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QFormLayout;

namespace windstille {

class Document;
class GLWidget;

/** Shows and edits properties of the primary selected object. */
class PropertyPanel final : public QWidget
{
  Q_OBJECT

public:
  explicit PropertyPanel(QWidget* parent = nullptr);
  ~PropertyPanel() override;

  void set_document(Document* document);
  void set_gl_widget(GLWidget* widget) { m_gl_widget = widget; }

public slots:
  void refresh_from_selection();

private slots:
  void on_pos_x_changed(double v);
  void on_pos_y_changed(double v);
  void on_scale_x_changed(double v);
  void on_scale_y_changed(double v);
  void on_angle_changed(double v);
  void on_hflip_changed(int state);
  void on_vflip_changed(int state);
  void on_map_type_changed(int index);

private:
  Document* m_document;
  GLWidget* m_gl_widget;
  bool m_updating;

  QLabel* m_name_label;
  QDoubleSpinBox* m_pos_x;
  QDoubleSpinBox* m_pos_y;
  QDoubleSpinBox* m_scale_x;
  QDoubleSpinBox* m_scale_y;
  QDoubleSpinBox* m_angle;
  QCheckBox* m_hflip;
  QCheckBox* m_vflip;
  QComboBox* m_map_type;

  PropertyPanel(PropertyPanel const&) = delete;
  PropertyPanel& operator=(PropertyPanel const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
