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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_GL_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_GL_WIDGET_HPP

#include <memory>

#include <QOpenGLWidget>

#include <wstdisplay/view.hpp>

namespace wstdisplay {
class Renderer;
} // namespace wstdisplay

namespace windstille {

class EditorWindow;

/** Main level canvas (Qt counterpart of WindstilleWidget).
    First realized instance initialises the process-wide QtGLDevice. */
class GLWidget final : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit GLWidget(EditorWindow* editor, QWidget* parent = nullptr);
  ~GLWidget() override;

  wstdisplay::View& view() { return m_view; }
  wstdisplay::View const& view() const { return m_view; }

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  EditorWindow* m_editor;
  std::unique_ptr<wstdisplay::Renderer> m_renderer;
  wstdisplay::View m_view;

  GLWidget(GLWidget const&) = delete;
  GLWidget& operator=(GLWidget const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
