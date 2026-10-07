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

#ifndef HEADER_WINDSTILLE_EDITOR_QT_GL_DEVICE_HPP
#define HEADER_WINDSTILLE_EDITOR_QT_GL_DEVICE_HPP

#include <memory>

#include <wstdisplay/device.hpp>

class QOpenGLContext;

namespace windstille {

/** wstdisplay::Device bound to the Qt editor's current OpenGL
    context (owned by QOpenGLWidget). Created once from the first
    canvas's initializeGL. */
class QtGLDevice final
{
public:
  /** \a context must already be current. */
  explicit QtGLDevice(QOpenGLContext* context);
  ~QtGLDevice();

  QOpenGLContext* context() const { return m_context; }
  wstdisplay::Device& device() const { return *m_device; }

private:
  QOpenGLContext* m_context;
  std::unique_ptr<wstdisplay::Device> m_device;

  QtGLDevice(QtGLDevice const&) = delete;
  QtGLDevice& operator=(QtGLDevice const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
