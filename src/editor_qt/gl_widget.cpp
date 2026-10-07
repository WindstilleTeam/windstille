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

#include "editor_qt/gl_widget.hpp"

#include <QMouseEvent>
#include <QSurfaceFormat>
#include <QWheelEvent>

#include <surf/color.hpp>
#include <geom/point.hpp>
#include <wstdisplay/renderer.hpp>

#include "editor_qt/app.hpp"
#include "editor_qt/editor_window.hpp"
#include "editor_qt/gl_device.hpp"

namespace windstille {

GLWidget::GLWidget(EditorWindow* editor, QWidget* parent) :
  QOpenGLWidget(parent),
  m_editor(editor),
  m_renderer(),
  m_view()
{
  QSurfaceFormat format;
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  setFormat(format);

  setMinimumSize(320, 240);
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

GLWidget::~GLWidget()
{
  makeCurrent();
  m_renderer.reset();
  doneCurrent();
}

void
GLWidget::initializeGL()
{
  // First canvas creates the process-wide GL device on this context.
  if (!g_qt_app.has_gl()) {
    g_qt_app.init_gl(context());
  }

  m_renderer = std::make_unique<wstdisplay::Renderer>(g_qt_app.device());
}

void
GLWidget::resizeGL(int w, int h)
{
  m_view.set_size(geom::isize(w, h));
}

void
GLWidget::paintGL()
{
  if (!m_renderer) {
    return;
  }

  m_renderer->begin_frame(m_view.get_size());
  // Placeholder clear — sector drawing lands once Document is toolkit-agnostic.
  m_renderer->clear(surf::Color(0.18f, 0.18f, 0.22f));
  m_renderer->end_frame();
}

void
GLWidget::mousePressEvent(QMouseEvent* event)
{
  QOpenGLWidget::mousePressEvent(event);
}

void
GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseReleaseEvent(event);
}

void
GLWidget::mouseMoveEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseMoveEvent(event);
}

void
GLWidget::wheelEvent(QWheelEvent* event)
{
  // Zoom toward cursor — simple step for the scaffold.
  float const steps = event->angleDelta().y() / 120.0f;
  if (steps > 0.0f) {
    m_view.set_zoom(geom::fpoint(static_cast<float>(event->position().x()), static_cast<float>(event->position().y())), m_view.get_zoom() * 1.1f);
  } else if (steps < 0.0f) {
    m_view.set_zoom(geom::fpoint(static_cast<float>(event->position().x()), static_cast<float>(event->position().y())), m_view.get_zoom() / 1.1f);
  }
  update();
  event->accept();
}

} // namespace windstille

/* EOF */
