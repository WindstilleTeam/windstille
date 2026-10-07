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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_APP_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_APP_HPP

#include <memory>
#include <string>

class QOpenGLContext;

#include <wstdisplay/fwd.hpp>

namespace windstille {

namespace sprite3d {
class Manager;
} // namespace sprite3d

class QtGLDevice;
class SpriteManager;

/** Process-wide services for the Qt editor (GL device, texture/surface
    managers). Constructed after the first OpenGL surface exists. */
class QtApp
{
public:
  QtApp();
  ~QtApp();

  void init_gl(QOpenGLContext* context);
  void shutdown_gl();

  bool has_gl() const { return m_gl_device != nullptr; }

  QtGLDevice& gl_device() const;
  wstdisplay::Device& device() const;
  wstdisplay::TextureManager& texture() const;
  wstdisplay::SurfaceManager& surface() const;
  SpriteManager& sprite() const;
  sprite3d::Manager& sprite3d() const;

  std::string const& datadir() const { return m_datadir; }
  void set_datadir(std::string const& dir) { m_datadir = dir; }

private:
  std::string m_datadir;
  std::unique_ptr<QtGLDevice> m_gl_device;
  std::unique_ptr<wstdisplay::TextureManager> m_texture_manager;
  std::unique_ptr<wstdisplay::SurfaceManager> m_surface_manager;
  std::unique_ptr<SpriteManager> m_sprite_manager;
  std::unique_ptr<sprite3d::Manager> m_sprite3d_manager;

  QtApp(QtApp const&) = delete;
  QtApp& operator=(QtApp const&) = delete;
};

extern QtApp g_qt_app;

} // namespace windstille

#endif

/* EOF */
