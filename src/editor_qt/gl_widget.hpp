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
#include <string>
#include <unordered_map>
#include <vector>

#include <QOpenGLWidget>
#include <QPointF>

#include <geom/rect.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/view.hpp>

#include "display/scene_context.hpp"
#include "editor/control_point.hpp"
#include "editor/document.hpp"
#include "editor/select_mask.hpp"

namespace wstdisplay {
class Renderer;
} // namespace wstdisplay

namespace windstille {

class EditorWindow;

/** Main level canvas (Qt counterpart of WindstilleWidget). */
class GLWidget final : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit GLWidget(EditorWindow* editor, QWidget* parent = nullptr);
  ~GLWidget() override;

  Document& document() { return *m_document; }
  Document const& document() const { return *m_document; }

  void load_file(std::string const& filename);
  void new_document();
  bool save_file(std::string const& filename);

  std::string const& filename() const { return m_filename; }
  void set_filename(std::string const& path) { m_filename = path; }

  SelectMask const& select_mask() const { return m_select_mask; }

  wstdisplay::View& view() { return m_view; }
  wstdisplay::View const& view() const { return m_view; }

  glm::vec2 screen_to_world(QPointF const& pos) const;

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

private:
  enum class Mode {
    None,
    Pan,
    SelectBox,
    DragObject,
    ControlDrag
  };

  void draw_sector();
  void update_selection_from_box();

  EditorWindow* m_editor;
  std::unique_ptr<Document> m_document;
  std::unique_ptr<wstdisplay::Renderer> m_renderer;
  std::unique_ptr<SceneContext> m_scene_context;
  wstdisplay::View m_view;
  std::string m_filename;
  SelectMask m_select_mask;

  Mode m_mode;
  QPointF m_last_mouse;
  glm::vec2 m_click_world;
  geom::frect m_select_rect;
  bool m_shift;
  /** World positions at drag start for undo. */
  std::vector<std::pair<ObjectModelHandle, glm::vec2>> m_drag_origins;
  ControlPointHandle m_ctrl_point;
  wstdisplay::Canvas m_overlay;

  GLWidget(GLWidget const&) = delete;
  GLWidget& operator=(GLWidget const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
