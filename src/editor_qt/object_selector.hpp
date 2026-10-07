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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_OBJECT_SELECTOR_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_OBJECT_SELECTOR_HPP

#include <string>

#include <QWidget>

class QListWidget;
class QListWidgetItem;

namespace windstille {

class GLWidget;

/** Palette of decal images from the data tree; double-click places at
    the view centre, or the path can be read for canvas placement. */
class ObjectSelector final : public QWidget
{
  Q_OBJECT

public:
  explicit ObjectSelector(QWidget* parent = nullptr);
  ~ObjectSelector() override;

  void set_gl_widget(GLWidget* widget) { m_gl_widget = widget; }
  void refresh();

  /** Relative data path of the currently selected item (empty if none). */
  std::string selected_path() const;

signals:
  void path_activated(std::string const& path);

private slots:
  void on_item_activated(QListWidgetItem* item);

private:
  void add_directory(std::string const& relative_dir);

  GLWidget* m_gl_widget;
  QListWidget* m_list;

  ObjectSelector(ObjectSelector const&) = delete;
  ObjectSelector& operator=(ObjectSelector const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
