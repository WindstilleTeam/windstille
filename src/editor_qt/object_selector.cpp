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

#include "editor_qt/object_selector.hpp"

#include <algorithm>

#include <QAbstractItemView>
#include <QListView>
#include <QDrag>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMimeData>
#include <QToolBar>
#include <QVBoxLayout>

#include "editor/decal_object_model.hpp"
#include "editor/document.hpp"
#include "editor/selection.hpp"
#include "editor/layer.hpp"
#include "editor/sector_model.hpp"
#include "editor_qt/gl_widget.hpp"
#include "util/directory.hpp"
#include "util/pathname.hpp"

namespace windstille {

namespace {

/** List that exports the data-relative path under our MIME type. */
class DecalListWidget final : public QListWidget
{
public:
  using QListWidget::QListWidget;

protected:
  QMimeData* mimeData(QList<QListWidgetItem*> const items) const override
  {
    QMimeData* mime = QListWidget::mimeData(items);
    if (!items.isEmpty()) {
      QString const path = items.front()->data(Qt::UserRole).toString();
      mime->setText(path);
      mime->setData(QStringLiteral("application/x-windstille-decal"), path.toUtf8());
    }
    return mime;
  }

  QStringList mimeTypes() const override
  {
    return {QStringLiteral("application/x-windstille-decal"),
            QStringLiteral("text/plain")};
  }
};

} // namespace

ObjectSelector::ObjectSelector(QWidget* parent) :
  QWidget(parent),
  m_gl_widget(nullptr),
  m_list(new DecalListWidget(this))
{
  m_list->setViewMode(QListView::ListMode);
  m_list->setUniformItemSizes(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->setDragEnabled(true);
  m_list->setDragDropMode(QAbstractItemView::DragOnly);
  m_list->setDefaultDropAction(Qt::CopyAction);

  auto* toolbar = new QToolBar(this);
  toolbar->addAction(tr("Refresh"), this, &ObjectSelector::refresh);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(new QLabel(tr("Objects"), this));
  layout->addWidget(toolbar);
  layout->addWidget(m_list);

  connect(m_list, &QListWidget::itemActivated, this, &ObjectSelector::on_item_activated);

  setMinimumWidth(180);
  setMaximumWidth(320);

  refresh();
}

ObjectSelector::~ObjectSelector() = default;

void
ObjectSelector::add_directory(std::string const& relative_dir)
{
  Pathname dir(relative_dir, Pathname::kDataPath);
  Directory::List images = Directory::read(dir, ".png");
  std::sort(images.begin(), images.end(),
            [](Pathname const& a, Pathname const& b) {
              return a.get_raw_path() < b.get_raw_path();
            });

  for (auto const& image : images) {
    auto* item = new QListWidgetItem(QString::fromStdString(image.get_raw_path()), m_list);
    item->setData(Qt::UserRole, QString::fromStdString(image.get_raw_path()));
    item->setToolTip(QString::fromStdString(image.get_sys_path()));
  }
}

void
ObjectSelector::refresh()
{
  m_list->clear();
  // Same roots as the Gtk ObjectSelector::populate().
  add_directory("images/decal/");
  add_directory("images/objects/bar/");
  add_directory("images/objects/");
}

std::string
ObjectSelector::selected_path() const
{
  auto* item = m_list->currentItem();
  if (!item) {
    return {};
  }
  return item->data(Qt::UserRole).toString().toStdString();
}

void
ObjectSelector::on_item_activated(QListWidgetItem* item)
{
  if (!item || !m_gl_widget) {
    return;
  }
  std::string const path = item->data(Qt::UserRole).toString().toStdString();
  if (path.empty()) {
    return;
  }

  Document& doc = m_gl_widget->document();
  SectorModel& sector = doc.get_sector_model();
  auto const& layers = sector.get_layers();
  if (layers.empty()) {
    return;
  }
  // Place on the topmost layer (last in draw order).
  LayerHandle layer = layers.back();

  geom::fpoint const center = m_gl_widget->view().get_pos();
  glm::vec2 const pos(center.x(), center.y());

  ObjectModelHandle object = DecalObjectModel::create(
    path, pos, path, DecalObjectModel::COLORMAP);
  doc.object_add(layer, object);

  SelectionHandle sel = Selection::create();
  sel->add(object);
  doc.set_selection(sel);
  m_gl_widget->update();

  emit path_activated(path);
}

} // namespace windstille

/* EOF */
