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

#include "editor_qt/editor_window.hpp"

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>

#include "editor_qt/gl_widget.hpp"
#include "editor_qt/layer_panel.hpp"
#include "editor_qt/object_selector.hpp"
#include "editor_qt/property_panel.hpp"

namespace windstille {

EditorWindow::EditorWindow(QWidget* parent) :
  QMainWindow(parent),
  m_tabs(nullptr),
  m_layer_panel(nullptr),
  m_object_selector(nullptr),
  m_property_panel(nullptr)
{
  setWindowTitle(QStringLiteral("Windstille Editor (Qt)"));
  resize(1400, 850);

  m_tabs = new QTabWidget(this);
  m_tabs->setTabsClosable(true);
  m_tabs->setMovable(true);
  m_tabs->setDocumentMode(true);
  connect(m_tabs, &QTabWidget::currentChanged, this, &EditorWindow::on_tab_changed);
  connect(m_tabs, &QTabWidget::tabCloseRequested, this, &EditorWindow::on_tab_close_requested);

  m_layer_panel = new LayerPanel(this);
  m_object_selector = new ObjectSelector(this);
  m_property_panel = new PropertyPanel(this);

  auto* left = new QSplitter(Qt::Vertical, this);
  left->addWidget(m_layer_panel);
  left->addWidget(m_object_selector);
  left->setStretchFactor(0, 1);
  left->setStretchFactor(1, 2);

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  splitter->addWidget(left);
  splitter->addWidget(m_tabs);
  splitter->addWidget(m_property_panel);
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);
  splitter->setStretchFactor(2, 0);
  splitter->setSizes({220, 960, 220});
  setCentralWidget(splitter);

  build_menus();
  build_toolbar();
  statusBar()->showMessage(QStringLiteral(
    "Select object then click canvas · LMB select/drag · Ctrl snap · MMB pan · Wheel zoom"));

  add_tab();
  update_title();
}

EditorWindow::~EditorWindow() = default;

GLWidget*
EditorWindow::gl_widget() const
{
  return qobject_cast<GLWidget*>(m_tabs->currentWidget());
}

QString
EditorWindow::tab_label_for(GLWidget* widget) const
{
  if (!widget || widget->filename().empty()) {
    return tr("Untitled");
  }
  return QFileInfo(QString::fromStdString(widget->filename())).fileName();
}

GLWidget*
EditorWindow::add_tab(std::string const& filename)
{
  auto* canvas = new GLWidget(this, m_tabs);
  if (!filename.empty()) {
    canvas->load_file(filename);
  }
  int const idx = m_tabs->addTab(canvas, tab_label_for(canvas));
  m_tabs->setCurrentIndex(idx);
  m_object_selector->set_gl_widget(canvas);
  sync_side_panels();
  return canvas;
}

void
EditorWindow::sync_side_panels()
{
  GLWidget* canvas = gl_widget();
  if (!canvas) {
    m_layer_panel->set_document(nullptr);
    m_property_panel->set_document(nullptr);
    m_object_selector->set_gl_widget(nullptr);
    return;
  }
  m_layer_panel->set_gl_widget(canvas);
  m_layer_panel->set_document(&canvas->document());
  m_property_panel->set_gl_widget(canvas);
  m_property_panel->set_document(&canvas->document());
  m_object_selector->set_gl_widget(canvas);
}

void
EditorWindow::build_menus()
{
  QMenu* file_menu = menuBar()->addMenu(tr("&File"));

  QAction* act_new = file_menu->addAction(tr("&New"), this, &EditorWindow::on_new);
  act_new->setShortcut(QKeySequence::New);

  QAction* act_open = file_menu->addAction(tr("&Open…"), this, &EditorWindow::on_open);
  act_open->setShortcut(QKeySequence::Open);

  QAction* act_save = file_menu->addAction(tr("&Save"), this, &EditorWindow::on_save);
  act_save->setShortcut(QKeySequence::Save);

  QAction* act_save_as = file_menu->addAction(tr("Save &As…"), this, &EditorWindow::on_save_as);
  act_save_as->setShortcut(QKeySequence::SaveAs);

  file_menu->addSeparator();
  QAction* act_close = file_menu->addAction(tr("&Close Tab"), this, &EditorWindow::on_close_tab);
  act_close->setShortcut(QKeySequence::Close);

  file_menu->addSeparator();
  QAction* act_quit = file_menu->addAction(tr("&Quit"), this, &EditorWindow::on_quit);
  act_quit->setShortcut(QKeySequence::Quit);

  QMenu* edit_menu = menuBar()->addMenu(tr("&Edit"));
  QAction* act_undo = edit_menu->addAction(tr("&Undo"), this, &EditorWindow::on_undo);
  act_undo->setShortcut(QKeySequence::Undo);
  QAction* act_redo = edit_menu->addAction(tr("&Redo"), this, &EditorWindow::on_redo);
  act_redo->setShortcut(QKeySequence::Redo);
  edit_menu->addSeparator();
  QAction* act_sel_all = edit_menu->addAction(tr("Select &All"), this, &EditorWindow::on_select_all);
  act_sel_all->setShortcut(QKeySequence::SelectAll);
  QAction* act_del = edit_menu->addAction(tr("&Delete"), this, &EditorWindow::on_delete);
  act_del->setShortcut(QKeySequence::Delete);

  QMenu* help_menu = menuBar()->addMenu(tr("&Help"));
  help_menu->addAction(tr("&About"), this, &EditorWindow::on_about);
}

void
EditorWindow::build_toolbar()
{
  QToolBar* tb = addToolBar(tr("Main"));
  tb->setMovable(false);
  tb->addAction(tr("New"), this, &EditorWindow::on_new);
  tb->addAction(tr("Open"), this, &EditorWindow::on_open);
  tb->addAction(tr("Save"), this, &EditorWindow::on_save);
}

void
EditorWindow::update_title()
{
  GLWidget* canvas = gl_widget();
  QString name = canvas ? tab_label_for(canvas) : tr("Untitled");
  setWindowTitle(tr("%1 — Windstille Editor (Qt)").arg(name));
  if (canvas) {
    int const idx = m_tabs->currentIndex();
    if (idx >= 0) {
      m_tabs->setTabText(idx, tab_label_for(canvas));
    }
  }
}

void
EditorWindow::load_file(std::string const& filename)
{
  add_tab(filename);
  update_title();
}

void
EditorWindow::on_tab_changed(int /*index*/)
{
  sync_side_panels();
  update_title();
}

void
EditorWindow::on_tab_close_requested(int index)
{
  if (index < 0) {
    return;
  }
  QWidget* w = m_tabs->widget(index);
  m_tabs->removeTab(index);
  delete w;
  if (m_tabs->count() == 0) {
    add_tab();
  }
  sync_side_panels();
  update_title();
}

void
EditorWindow::on_close_tab()
{
  on_tab_close_requested(m_tabs->currentIndex());
}

void
EditorWindow::on_new()
{
  add_tab();
  update_title();
  statusBar()->showMessage(tr("New sector"), 3000);
}

void
EditorWindow::on_open()
{
  QString const path = QFileDialog::getOpenFileName(
    this,
    tr("Open Sector"),
    QString(),
    tr("Windstille sectors (*.sexp *.txt);;All files (*)"));
  if (path.isEmpty()) {
    return;
  }
  add_tab(path.toStdString());
  update_title();
  statusBar()->showMessage(tr("Opened %1").arg(path), 5000);
}

void
EditorWindow::on_save()
{
  GLWidget* canvas = gl_widget();
  if (!canvas) {
    return;
  }
  if (canvas->filename().empty()) {
    on_save_as();
    return;
  }
  if (canvas->save_file(canvas->filename())) {
    update_title();
    statusBar()->showMessage(tr("Saved %1")
      .arg(QString::fromStdString(canvas->filename())), 5000);
  } else {
    QMessageBox::warning(this, tr("Save failed"),
                         tr("Could not write the sector file."));
  }
}

void
EditorWindow::on_save_as()
{
  GLWidget* canvas = gl_widget();
  if (!canvas) {
    return;
  }
  QString const path = QFileDialog::getSaveFileName(
    this,
    tr("Save Sector"),
    QString::fromStdString(canvas->filename()),
    tr("Windstille sectors (*.sexp *.txt);;All files (*)"));
  if (path.isEmpty()) {
    return;
  }
  if (canvas->save_file(path.toStdString())) {
    update_title();
    statusBar()->showMessage(tr("Saved %1").arg(path), 5000);
  } else {
    QMessageBox::warning(this, tr("Save failed"),
                         tr("Could not write the sector file."));
  }
}

void
EditorWindow::on_undo()
{
  if (GLWidget* canvas = gl_widget()) {
    canvas->document().undo();
    canvas->update();
  }
}

void
EditorWindow::on_redo()
{
  if (GLWidget* canvas = gl_widget()) {
    canvas->document().redo();
    canvas->update();
  }
}

void
EditorWindow::on_delete()
{
  if (GLWidget* canvas = gl_widget()) {
    canvas->document().selection_delete();
    canvas->update();
  }
}

void
EditorWindow::on_select_all()
{
  if (GLWidget* canvas = gl_widget()) {
    canvas->document().select_all();
    canvas->update();
  }
}

void
EditorWindow::on_quit()
{
  close();
}

void
EditorWindow::on_about()
{
  QMessageBox::about(
    this,
    tr("About Windstille Editor (Qt)"),
    tr("Windstille level editor — Qt6 port.\n"
       "Runs side-by-side with the Gtk editor for comparison.\n\n"
       "Multi-document tabs · select/drag/scale/rotate · layers · properties\n\n"
       "GPLv3+"));
}

} // namespace windstille

/* EOF */
