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

#include <algorithm>
#include <fstream>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QSurfaceFormat>
#include <QWheelEvent>

#include <geom/point.hpp>
#include <logmich/log.hpp>
#include <surf/color.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/renderer.hpp>

#include "editor_qt/app.hpp"
#include "editor_qt/editor_window.hpp"
#include "editor/selection.hpp"
#include "util/file_writer.hpp"

namespace windstille {

GLWidget::GLWidget(EditorWindow* editor, QWidget* parent) :
  QOpenGLWidget(parent),
  m_editor(editor),
  m_document(std::make_unique<Document>()),
  m_renderer(),
  m_scene_context(std::make_unique<SceneContext>()),
  m_view(),
  m_filename(),
  m_select_mask(),
  m_mode(Mode::None),
  m_last_mouse(),
  m_click_world(),
  m_select_rect(),
  m_shift(false)
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

  m_scene_context->set_render_mask(
    m_scene_context->get_render_mask() & ~SceneContext::LIGHTMAP);

  m_document->signal_on_change().connect([this]() { update(); });
}

GLWidget::~GLWidget()
{
  makeCurrent();
  m_renderer.reset();
  doneCurrent();
}

void
GLWidget::new_document()
{
  m_document = std::make_unique<Document>();
  m_filename.clear();
  m_document->signal_on_change().connect([this]() { update(); });
  update();
}

void
GLWidget::load_file(std::string const& filename)
{
  try {
    m_document = std::make_unique<Document>(filename);
    m_filename = filename;
    m_document->signal_on_change().connect([this]() { update(); });
    logmich::info("loaded {}", filename);
  } catch (std::exception const& err) {
    logmich::error("failed to load {}: {}", filename, err.what());
  }
  update();
}

bool
GLWidget::save_file(std::string const& path)
{
  try {
    std::ofstream out(path.c_str());
    if (!out) {
      logmich::error("cannot open {} for writing", path);
      return false;
    }
    FileWriter writer(out);
    m_document->get_sector_model().write(writer);
    m_filename = path;
    logmich::info("wrote {}", path);
    return true;
  } catch (std::exception const& err) {
    logmich::error("save failed: {}", err.what());
    return false;
  }
}

glm::vec2
GLWidget::screen_to_world(QPointF const& pos) const
{
  float const dpr = static_cast<float>(devicePixelRatioF());
  return m_view.screen_to_world(geom::fpoint(
    static_cast<float>(pos.x()) * dpr,
    static_cast<float>(pos.y()) * dpr)).as_vec();
}

void
GLWidget::initializeGL()
{
  if (!g_qt_app.has_gl()) {
    g_qt_app.init_gl(context());
  }
  m_renderer = std::make_unique<wstdisplay::Renderer>(g_qt_app.device());
}

void
GLWidget::resizeGL(int w, int h)
{
  geom::fpoint const top_left = m_view.screen_to_world(geom::fpoint(0.0f, 0.0f));
  m_view.set_size(geom::isize(w, h));
  m_view.set_pos(top_left + geom::foffset(
    static_cast<float>(w) / 2.0f / m_view.get_zoom(),
    static_cast<float>(h) / 2.0f / m_view.get_zoom()));
}

void
GLWidget::paintGL()
{
  if (!m_renderer) {
    return;
  }

  m_renderer->begin_frame(m_view.get_size());
  m_renderer->clear(surf::Color(0.18f, 0.18f, 0.22f));
  draw_sector();
  m_scene_context->render(*m_renderer, m_view);
  m_renderer->end_frame();
}

void
GLWidget::draw_sector()
{
  SectorModel& sector = m_document->get_sector_model();
  m_scene_context->light().fill_screen(sector.get_ambient_color());
  {
    wstdisplay::Canvas& color = m_scene_context->color();
    wstdisplay::Canvas::Scope scope(color);
    color.set_z(-1000.0f);
    color.set_space(wstdisplay::Space::Screen);
    color.fill_screen(surf::Color(0.12f, 0.12f, 0.14f));
  }
  sector.draw_content(*m_scene_context);
  sector.draw(*m_scene_context, m_select_mask);

  SelectionHandle sel = m_document->get_selection();
  if (sel && !sel->empty()) {
    bool first = true;
    for (auto it = sel->begin(); it != sel->end(); ++it) {
      (*it)->draw_select(*m_scene_context, first);
      first = false;
    }
  }

  if (m_mode == Mode::SelectBox) {
    wstdisplay::Canvas& control = m_scene_context->control();
    wstdisplay::Canvas::Scope scope(control);
    control.draw_rect(m_select_rect, surf::Color(1.0f, 1.0f, 1.0f, 0.9f));
  }
}

void
GLWidget::mousePressEvent(QMouseEvent* event)
{
  m_shift = event->modifiers() & Qt::ShiftModifier;
  m_last_mouse = event->position();
  m_click_world = screen_to_world(event->position());

  if (event->button() == Qt::MiddleButton ||
      (event->button() == Qt::LeftButton && (event->modifiers() & Qt::AltModifier))) {
    m_mode = Mode::Pan;
    setCursor(Qt::ClosedHandCursor);
    event->accept();
    return;
  }

  if (event->button() == Qt::LeftButton) {
    ObjectModelHandle object =
      m_document->get_sector_model().get_object_at(m_click_world, m_select_mask);

    if (object) {
      SelectionHandle sel = m_document->get_selection();
      if (!sel->has_object(object)) {
        if (m_shift) {
          sel->add(object);
        } else {
          SelectionHandle neu = Selection::create();
          neu->add(object);
          m_document->set_selection(neu);
        }
      } else if (m_shift) {
        sel->remove(object);
      }
      m_mode = Mode::DragObject;
    } else {
      if (!m_shift) {
        m_document->set_selection(Selection::create());
      }
      m_select_rect = geom::frect(
        m_click_world.x, m_click_world.y,
        m_click_world.x, m_click_world.y);
      m_mode = Mode::SelectBox;
    }
    update();
    event->accept();
    return;
  }

  QOpenGLWidget::mousePressEvent(event);
}

void
GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (m_mode == Mode::Pan &&
      (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
    m_mode = Mode::None;
    unsetCursor();
    event->accept();
    return;
  }

  if (event->button() == Qt::LeftButton) {
    if (m_mode == Mode::SelectBox) {
      update_selection_from_box();
    } else if (m_mode == Mode::DragObject) {
      m_document->create_control_points();
    }
    m_mode = Mode::None;
    update();
    event->accept();
    return;
  }

  QOpenGLWidget::mouseReleaseEvent(event);
}

void
GLWidget::mouseMoveEvent(QMouseEvent* event)
{
  QPointF const pos = event->position();
  glm::vec2 const world = screen_to_world(pos);

  if (m_mode == Mode::Pan) {
    float const dpr = static_cast<float>(devicePixelRatioF());
    float const dx = static_cast<float>(pos.x() - m_last_mouse.x()) * dpr;
    float const dy = static_cast<float>(pos.y() - m_last_mouse.y()) * dpr;
    geom::fpoint cam = m_view.get_pos();
    m_view.set_pos(geom::fpoint(
      cam.x() - dx / m_view.get_zoom(),
      cam.y() - dy / m_view.get_zoom()));
    m_last_mouse = pos;
    update();
    event->accept();
    return;
  }

  if (m_mode == Mode::SelectBox) {
    float const l = std::min(m_click_world.x, world.x);
    float const r = std::max(m_click_world.x, world.x);
    float const top = std::min(m_click_world.y, world.y);
    float const bot = std::max(m_click_world.y, world.y);
    m_select_rect = geom::frect(l, top, r, bot);
    update();
    event->accept();
    return;
  }

  if (m_mode == Mode::DragObject) {
    glm::vec2 const delta = world - m_click_world;
    m_click_world = world;
    SelectionHandle sel = m_document->get_selection();
    if (sel) {
      for (auto it = sel->begin(); it != sel->end(); ++it) {
        (*it)->set_rel_pos((*it)->get_rel_pos() + delta);
      }
      m_document->signal_on_change()();
    }
    update();
    event->accept();
    return;
  }

  QOpenGLWidget::mouseMoveEvent(event);
}

void
GLWidget::wheelEvent(QWheelEvent* event)
{
  float const steps = event->angleDelta().y() / 120.0f;
  geom::fpoint const pos(
    static_cast<float>(event->position().x()) * static_cast<float>(devicePixelRatioF()),
    static_cast<float>(event->position().y()) * static_cast<float>(devicePixelRatioF()));
  if (steps > 0.0f) {
    m_view.set_zoom(pos, m_view.get_zoom() * 1.1f);
  } else if (steps < 0.0f) {
    m_view.set_zoom(pos, m_view.get_zoom() / 1.1f);
  }
  update();
  event->accept();
}

void
GLWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    m_document->selection_delete();
    update();
    event->accept();
    return;
  }
  if (event->matches(QKeySequence::SelectAll)) {
    m_document->select_all();
    update();
    event->accept();
    return;
  }
  if (event->matches(QKeySequence::Undo)) {
    m_document->undo();
    update();
    event->accept();
    return;
  }
  if (event->matches(QKeySequence::Redo)) {
    m_document->redo();
    update();
    event->accept();
    return;
  }
  QOpenGLWidget::keyPressEvent(event);
}

void
GLWidget::update_selection_from_box()
{
  SelectionHandle box_sel =
    m_document->get_sector_model().get_selection(m_select_rect, m_select_mask);
  if (m_shift) {
    SelectionHandle cur = m_document->get_selection();
    cur->add(box_sel->begin(), box_sel->end());
  } else {
    m_document->set_selection(box_sel);
  }
}

} // namespace windstille

/* EOF */
