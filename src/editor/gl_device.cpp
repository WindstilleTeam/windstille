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

#include "editor/gl_device.hpp"

#include <dlfcn.h>

#include <stdexcept>
#include <string>

#include <gdkmm/window.h>

namespace windstille {

namespace {

/** Gtk uses libepoxy, which resolves GL functions for the current
    context on both GLX and EGL. epoxy_glFoo is a pointer to the
    dispatch function of glFoo. */
void* get_proc_address(char const* name)
{
  std::string const symbol = std::string("epoxy_") + name;
  void* const dispatch = dlsym(RTLD_DEFAULT, symbol.c_str());
  return dispatch ? *static_cast<void**>(dispatch) : nullptr;
}

} // namespace

GLDevice::GLDevice(Gtk::Window& window) :
  m_context(),
  m_device()
{
  Glib::RefPtr<Gdk::Window> gdk_window = window.get_window();
  if (!gdk_window) {
    throw std::runtime_error("GLDevice: window isn't realized");
  }

  m_context = gdk_window->create_gl_context();
  m_context->set_required_version(3, 3);
  m_context->realize();
  m_context->make_current();

  m_device = std::make_unique<wstdisplay::Device>(&get_proc_address);
}

GLDevice::~GLDevice()
{
  // resources are deleted with the context current
  m_context->make_current();
  m_device.reset();
}

} // namespace windstille

/* EOF */
