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

#include <QMainWindow>

namespace windstille {

class GLWidget;

/** Main window of the Qt-based Windstille level editor. */
class EditorWindow final : public QMainWindow
{
  Q_OBJECT

public:
  explicit EditorWindow(QWidget* parent = nullptr);
  ~EditorWindow() override;

  GLWidget* gl_widget() const { return m_gl_widget; }

private slots:
  void on_new();
  void on_open();
  void on_quit();
  void on_about();

private:
  void build_menus();
  void build_toolbar();

  GLWidget* m_gl_widget;

  EditorWindow(EditorWindow const&) = delete;
  EditorWindow& operator=(EditorWindow const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
