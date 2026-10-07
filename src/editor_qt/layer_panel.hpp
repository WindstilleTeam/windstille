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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_LAYER_PANEL_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_LAYER_PANEL_HPP

#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;
class QToolBar;

namespace windstille {

class Document;
class SectorModel;
class GLWidget;

/** Side panel listing sector layers (name, visible, locked). */
class LayerPanel final : public QWidget
{
  Q_OBJECT

public:
  explicit LayerPanel(QWidget* parent = nullptr);
  ~LayerPanel() override;

  void set_document(Document* document);
  void set_gl_widget(GLWidget* widget) { m_gl_widget = widget; }

  /** Layer currently selected in the tree (topmost first in UI). */
  class Layer* current_layer() const;

public slots:
  void rebuild();

private slots:
  void on_item_changed(QTreeWidgetItem* item, int column);
  void on_selection_changed();
  void on_new_layer();
  void on_delete_layer();
  void on_reverse_layers();
  void on_show_all();
  void on_hide_all();

private:
  Document* m_document;
  GLWidget* m_gl_widget;
  QTreeWidget* m_tree;
  QToolBar* m_toolbar;
  bool m_updating;

  LayerPanel(LayerPanel const&) = delete;
  LayerPanel& operator=(LayerPanel const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
