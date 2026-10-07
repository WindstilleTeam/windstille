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

#include "editor_qt/layer_panel.hpp"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QAbstractItemView>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QHeaderView>
#include <QVariant>

#include "editor/document.hpp"
#include "editor/layer.hpp"
#include "editor/sector_model.hpp"
#include "editor_qt/gl_widget.hpp"

namespace windstille {

namespace {
constexpr int kRoleLayerPtr = Qt::UserRole;
}

LayerPanel::LayerPanel(QWidget* parent) :
  QWidget(parent),
  m_document(nullptr),
  m_gl_widget(nullptr),
  m_tree(new QTreeWidget(this)),
  m_toolbar(new QToolBar(this)),
  m_updating(false)
{
  m_tree->setColumnCount(3);
  m_tree->setHeaderLabels({tr("Name"), tr("Vis"), tr("Lock")});
  m_tree->header()->setStretchLastSection(false);
  m_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  m_tree->setRootIsDecorated(false);
  m_tree->setSelectionMode(QAbstractItemView::SingleSelection);

  m_toolbar->setIconSize(QSize(16, 16));
  m_toolbar->addAction(tr("New"), this, &LayerPanel::on_new_layer);
  m_toolbar->addAction(tr("Del"), this, &LayerPanel::on_delete_layer);
  m_toolbar->addAction(tr("Rev"), this, &LayerPanel::on_reverse_layers);
  m_toolbar->addSeparator();
  m_toolbar->addAction(tr("Show"), this, &LayerPanel::on_show_all);
  m_toolbar->addAction(tr("Hide"), this, &LayerPanel::on_hide_all);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_toolbar);
  layout->addWidget(m_tree);

  connect(m_tree, &QTreeWidget::itemChanged, this, &LayerPanel::on_item_changed);
  connect(m_tree, &QTreeWidget::itemSelectionChanged, this, &LayerPanel::on_selection_changed);

  setMinimumWidth(180);
  setMaximumWidth(320);
}

LayerPanel::~LayerPanel() = default;

void
LayerPanel::set_document(Document* document)
{
  m_document = document;
  rebuild();
  if (m_document) {
    m_document->get_sector_model().signal_layers_changed().connect([this]() {
      rebuild();
    });
  }
}

Layer*
LayerPanel::current_layer() const
{
  auto* item = m_tree->currentItem();
  if (!item) {
    return nullptr;
  }
  return reinterpret_cast<Layer*>(item->data(0, kRoleLayerPtr).value<quintptr>());
}

void
LayerPanel::rebuild()
{
  m_updating = true;
  m_tree->clear();
  if (!m_document) {
    m_updating = false;
    return;
  }

  auto const& layers = m_document->get_sector_model().get_layers();
  // UI lists topmost first (reverse of draw order).
  for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
    LayerHandle layer = *it;
    auto* item = new QTreeWidgetItem(m_tree);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
    item->setText(0, QString::fromStdString(layer->get_name()));
    item->setCheckState(1, layer->is_visible() ? Qt::Checked : Qt::Unchecked);
    item->setCheckState(2, layer->is_locked() ? Qt::Checked : Qt::Unchecked);
    item->setData(0, kRoleLayerPtr, QVariant::fromValue(
      reinterpret_cast<quintptr>(layer.get())));
  }

  if (m_tree->topLevelItemCount() > 0) {
    m_tree->setCurrentItem(m_tree->topLevelItem(0));
  }
  m_updating = false;
}

void
LayerPanel::on_item_changed(QTreeWidgetItem* item, int column)
{
  if (m_updating || !item || !m_document) {
    return;
  }
  auto* layer = reinterpret_cast<Layer*>(item->data(0, kRoleLayerPtr).value<quintptr>());
  if (!layer) {
    return;
  }
  if (column == 0) {
    layer->set_name(item->text(0).toStdString());
  } else if (column == 1) {
    layer->set_visible(item->checkState(1) == Qt::Checked);
  } else if (column == 2) {
    layer->set_locked(item->checkState(2) == Qt::Checked);
  }
  if (m_gl_widget) {
    m_gl_widget->update();
  }
  m_document->signal_on_change()();
}

void
LayerPanel::on_selection_changed()
{
  // Current layer is queried by the canvas when placing objects.
}

void
LayerPanel::on_new_layer()
{
  if (!m_document) {
    return;
  }
  m_document->layer_add();
  rebuild();
}

void
LayerPanel::on_delete_layer()
{
  if (!m_document) {
    return;
  }
  Layer* layer = current_layer();
  if (!layer) {
    return;
  }
  // Find shared_ptr in sector
  for (auto const& handle : m_document->get_sector_model().get_layers()) {
    if (handle.get() == layer) {
      m_document->layer_remove(handle);
      break;
    }
  }
  rebuild();
}

void
LayerPanel::on_reverse_layers()
{
  if (!m_document) {
    return;
  }
  m_document->get_sector_model().reverse_layers();
  rebuild();
}

void
LayerPanel::on_show_all()
{
  if (!m_document) {
    return;
  }
  m_document->get_sector_model().set_all_visible(true);
  rebuild();
  if (m_gl_widget) {
    m_gl_widget->update();
  }
}

void
LayerPanel::on_hide_all()
{
  if (!m_document) {
    return;
  }
  m_document->get_sector_model().set_all_visible(false);
  rebuild();
  if (m_gl_widget) {
    m_gl_widget->update();
  }
}

} // namespace windstille

/* EOF */
