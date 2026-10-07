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
#include <QStatusBar>
#include <QToolBar>

#include "editor_qt/gl_widget.hpp"
#include "editor_qt/layer_panel.hpp"

#include <QSplitter>

namespace windstille {

EditorWindow::EditorWindow(QWidget* parent) :
  QMainWindow(parent),
  m_gl_widget(nullptr),
  m_layer_panel(nullptr)
{
  setWindowTitle(QStringLiteral("Windstille Editor (Qt)"));
  resize(1280, 800);

  m_gl_widget = new GLWidget(this, this);
  m_layer_panel = new LayerPanel(this);
  m_layer_panel->set_gl_widget(m_gl_widget);
  m_layer_panel->set_document(&m_gl_widget->document());

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  splitter->addWidget(m_layer_panel);
  splitter->addWidget(m_gl_widget);
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);
  splitter->setSizes({220, 1060});
  setCentralWidget(splitter);

  build_menus();
  build_toolbar();
  statusBar()->showMessage(QStringLiteral(
    "LMB select/drag · Shift add · Ctrl snap · MMB/Alt+LMB pan · Wheel zoom · Del delete"));
  update_title();
}

EditorWindow::~EditorWindow() = default;

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
  QString name = QStringLiteral("Untitled");
  if (m_gl_widget && !m_gl_widget->filename().empty()) {
    name = QFileInfo(QString::fromStdString(m_gl_widget->filename())).fileName();
  }
  setWindowTitle(tr("%1 — Windstille Editor (Qt)").arg(name));
}

void
EditorWindow::load_file(std::string const& filename)
{
  if (m_gl_widget) {
    m_gl_widget->load_file(filename);
  }
  if (m_layer_panel && m_gl_widget) {
    m_layer_panel->set_document(&m_gl_widget->document());
  }
  update_title();
}

void
EditorWindow::on_new()
{
  if (m_gl_widget) {
    m_gl_widget->new_document();
  }
  if (m_layer_panel && m_gl_widget) {
    m_layer_panel->set_document(&m_gl_widget->document());
  }
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
  if (m_gl_widget) {
    m_gl_widget->load_file(path.toStdString());
  }
  if (m_layer_panel && m_gl_widget) {
    m_layer_panel->set_document(&m_gl_widget->document());
  }
  update_title();
  statusBar()->showMessage(tr("Opened %1").arg(path), 5000);
}

void
EditorWindow::on_save()
{
  if (!m_gl_widget) {
    return;
  }
  if (m_gl_widget->filename().empty()) {
    on_save_as();
    return;
  }
  if (m_gl_widget->save_file(m_gl_widget->filename())) {
    statusBar()->showMessage(tr("Saved %1")
      .arg(QString::fromStdString(m_gl_widget->filename())), 5000);
  } else {
    QMessageBox::warning(this, tr("Save failed"),
                         tr("Could not write the sector file."));
  }
}

void
EditorWindow::on_save_as()
{
  if (!m_gl_widget) {
    return;
  }
  QString const path = QFileDialog::getSaveFileName(
    this,
    tr("Save Sector"),
    QString::fromStdString(m_gl_widget->filename()),
    tr("Windstille sectors (*.sexp *.txt);;All files (*)"));
  if (path.isEmpty()) {
    return;
  }
  if (m_gl_widget->save_file(path.toStdString())) {
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
  if (m_gl_widget) {
    m_gl_widget->document().undo();
    m_gl_widget->update();
  }
}

void
EditorWindow::on_redo()
{
  if (m_gl_widget) {
    m_gl_widget->document().redo();
    m_gl_widget->update();
  }
}

void
EditorWindow::on_delete()
{
  if (m_gl_widget) {
    m_gl_widget->document().selection_delete();
    m_gl_widget->update();
  }
}

void
EditorWindow::on_select_all()
{
  if (m_gl_widget) {
    m_gl_widget->document().select_all();
    m_gl_widget->update();
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
       "LMB: select / drag objects\n"
       "Shift+LMB: add to selection\n"
       "MMB or Alt+LMB: pan\n"
       "Wheel: zoom\n"
       "Del: delete selection\n\n"
       "GPLv3+"));
}

} // namespace windstille

/* EOF */
