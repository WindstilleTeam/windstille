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

#include "editor_qt/gl_device.hpp"

#include <stdexcept>

#include <QOpenGLContext>

namespace windstille {

namespace {

void* get_proc_address(char const* name)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) {
    return nullptr;
  }
  return reinterpret_cast<void*>(ctx->getProcAddress(name));
}

} // namespace

QtGLDevice::QtGLDevice(QOpenGLContext* context) :
  m_context(context),
  m_device()
{
  if (!context || !context->isValid()) {
    throw std::runtime_error("QtGLDevice: invalid QOpenGLContext");
  }
  if (QOpenGLContext::currentContext() != context) {
    throw std::runtime_error("QtGLDevice: context is not current");
  }

  m_device = std::make_unique<wstdisplay::Device>(&get_proc_address);
}

QtGLDevice::~QtGLDevice()
{
  // Caller must hold the context current while destroying GL resources.
  m_device.reset();
}

} // namespace windstille

/* EOF */
