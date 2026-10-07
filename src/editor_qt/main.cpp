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

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <QApplication>
#include <QSurfaceFormat>

#include <argpp/argpp.hpp>
#include <logmich/log.hpp>

#include "editor_qt/app.hpp"
#include "editor_qt/editor_window.hpp"
#include "util/pathname.hpp"
#include "util/system.hpp"

namespace windstille {

static int run_editor(int argc, char** argv)
{
  std::string datadir;
  std::vector<std::string> rest_args;

  argpp::Parser argp;
  argp.add_usage(argv[0], "[LEVELFILE]")
    .add_text("Windstille Level Editor (Qt)");

  argp.add_group()
    .add_option('h', "help", "", "Print this help")
    .add_option('d', "datadir", "DIR", "Fetch game data from DIR")
    .add_option('D', "debug", "", "Print debug level messages");

  for (auto const& opt : argp.parse_args(argc, argv)) {
    switch (opt.key) {
      case 'D':
        logmich::g_logger.set_log_level(logmich::LogLevel::TRACE);
        break;
      case 'd':
        datadir = opt.argument;
        break;
      case 'h':
        argp.print_help();
        return EXIT_SUCCESS;
      case argpp::ArgumentType::REST:
        rest_args.push_back(opt.argument);
        break;
      default:
        break;
    }
  }

  if (datadir.empty()) {
    datadir = System::find_default_datadir();
  }
  g_qt_app.set_datadir(datadir);
  Pathname::set_datadir(datadir);

  // Request a core profile before any QOpenGLWidget is created.
  QSurfaceFormat format;
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  QSurfaceFormat::setDefaultFormat(format);

  QApplication qapp(argc, argv);
  QApplication::setApplicationName(QStringLiteral("windstille-editor-qt"));
  QApplication::setOrganizationName(QStringLiteral("Windstille"));

  EditorWindow window;
  window.show();

  if (!rest_args.empty()) {
    // Path will be opened once SectorModel is toolkit-agnostic.
    logmich::info("level argument present but loading not yet wired: {}", rest_args.front());
  }

  return qapp.exec();
}

} // namespace windstille

int main(int argc, char** argv)
{
  try {
    return windstille::run_editor(argc, argv);
  } catch (std::exception const& err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}

/* EOF */
