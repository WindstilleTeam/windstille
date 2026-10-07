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
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include "editor_qt/gl_widget.hpp"

namespace windstille {

EditorWindow::EditorWindow(QWidget* parent) :
  QMainWindow(parent),
  m_gl_widget(nullptr)
{
  setWindowTitle(QStringLiteral("Windstille Editor (Qt)"));
  resize(1280, 800);

  m_gl_widget = new GLWidget(this, this);
  setCentralWidget(m_gl_widget);

  build_menus();
  build_toolbar();
  statusBar()->showMessage(QStringLiteral("Ready"));
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

  file_menu->addSeparator();
  QAction* act_quit = file_menu->addAction(tr("&Quit"), this, &EditorWindow::on_quit);
  act_quit->setShortcut(QKeySequence::Quit);

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
}

void
EditorWindow::on_new()
{
  if (m_gl_widget) m_gl_widget->new_document();
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
  if (m_gl_widget) m_gl_widget->load_file(path.toStdString());
  statusBar()->showMessage(tr("Opened %1").arg(path), 5000);
}

void
EditorWindow::load_file(std::string const& filename)
{
  if (m_gl_widget) m_gl_widget->load_file(filename);
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
       "Runs side-by-side with the Gtk editor for comparison.\n"
       "GPLv3+"));
}

} // namespace windstille

/* EOF */
