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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_EDITOR_WINDOW_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_EDITOR_WINDOW_HPP

#include <string>

#include <QMainWindow>

class QTabWidget;

namespace windstille {

class GLWidget;
class LayerPanel;
class ObjectSelector;
class PropertyPanel;
class TimelinePanel;

/** Main window of the Qt-based Windstille level editor. */
class EditorWindow final : public QMainWindow
{
  Q_OBJECT

public:
  explicit EditorWindow(QWidget* parent = nullptr);
  ~EditorWindow() override;

  /** Active canvas (current tab); may be null if no tabs. */
  GLWidget* gl_widget() const;
  ObjectSelector* object_selector() const { return m_object_selector; }
  void load_file(std::string const& filename);

private slots:
  void on_new();
  void on_open();
  void on_save();
  void on_save_as();
  void on_close_tab();
  void on_quit();
  void on_about();
  void on_undo();
  void on_redo();
  void on_delete();
  void on_select_all();
  void on_toggle_grid(bool checked);
  void on_toggle_background(bool checked);
  void on_zoom_to_fit();
  void on_zoom_reset();
  void on_tool_select();
  void on_tool_navgraph();
  void on_tab_changed(int index);
  void on_tab_close_requested(int index);

private:
  void build_menus();
  void build_toolbar();
  void update_title();
  void sync_side_panels();
  GLWidget* add_tab(std::string const& filename = {});
  QString tab_label_for(GLWidget* widget) const;

  QTabWidget* m_tabs;
  LayerPanel* m_layer_panel;
  ObjectSelector* m_object_selector;
  PropertyPanel* m_property_panel;
  TimelinePanel* m_timeline_panel;

  EditorWindow(EditorWindow const&) = delete;
  EditorWindow& operator=(EditorWindow const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
