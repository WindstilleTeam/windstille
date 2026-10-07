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

#include "editor_qt/app.hpp"

#include <stdexcept>

#include <QOpenGLContext>

#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>

#include "editor_qt/gl_device.hpp"
#include "sprite2d/manager.hpp"
#include "sprite3d/manager.hpp"

namespace windstille {

QtApp g_qt_app;

QtApp::QtApp() :
  m_datadir(),
  m_gl_device(),
  m_texture_manager(),
  m_surface_manager(),
  m_sprite_manager(),
  m_sprite3d_manager()
{
}

QtApp::~QtApp()
{
  shutdown_gl();
}

void
QtApp::init_gl(QOpenGLContext* context)
{
  if (m_gl_device) {
    return;
  }

  m_gl_device = std::make_unique<QtGLDevice>(context);
  m_texture_manager = std::make_unique<wstdisplay::TextureManager>(m_gl_device->device());
  m_surface_manager = std::make_unique<wstdisplay::SurfaceManager>(m_gl_device->device());
  m_sprite_manager = std::make_unique<SpriteManager>(*m_surface_manager);
  m_sprite3d_manager = std::make_unique<sprite3d::Manager>();
}

void
QtApp::shutdown_gl()
{
  m_sprite3d_manager.reset();
  m_sprite_manager.reset();
  m_surface_manager.reset();
  m_texture_manager.reset();
  m_gl_device.reset();
}

QtGLDevice&
QtApp::gl_device() const
{
  if (!m_gl_device) {
    throw std::runtime_error("QtApp: GL device not initialised");
  }
  return *m_gl_device;
}

wstdisplay::Device&
QtApp::device() const
{
  return gl_device().device();
}

wstdisplay::TextureManager&
QtApp::texture() const
{
  if (!m_texture_manager) {
    throw std::runtime_error("QtApp: texture manager not initialised");
  }
  return *m_texture_manager;
}

wstdisplay::SurfaceManager&
QtApp::surface() const
{
  if (!m_surface_manager) {
    throw std::runtime_error("QtApp: surface manager not initialised");
  }
  return *m_surface_manager;
}

SpriteManager&
QtApp::sprite() const
{
  if (!m_sprite_manager) {
    throw std::runtime_error("QtApp: sprite manager not initialised");
  }
  return *m_sprite_manager;
}

sprite3d::Manager&
QtApp::sprite3d() const
{
  if (!m_sprite3d_manager) {
    throw std::runtime_error("QtApp: sprite3d manager not initialised");
  }
  return *m_sprite3d_manager;
}

} // namespace windstille

/* EOF */
