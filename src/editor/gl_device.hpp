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

#ifndef HEADER_WINDSTILLE_EDITOR_GL_DEVICE_HPP
#define HEADER_WINDSTILLE_EDITOR_GL_DEVICE_HPP

#include <memory>

#include <gdkmm/glcontext.h>
#include <gtkmm/window.h>
#include <wstdisplay/device.hpp>

namespace windstille {

/** The OpenGL context shared by all WindstilleWidgets of the editor
    and the wstdisplay::Device on it, so that textures, sprites and
    fonts are loaded once for all open documents. */
class GLDevice final
{
public:
  /** Create the context on the realized \a window */
  explicit GLDevice(Gtk::Window& window);
  ~GLDevice();

  Glib::RefPtr<Gdk::GLContext> get_context() const { return m_context; }
  wstdisplay::Device& get_device() const { return *m_device; }

private:
  Glib::RefPtr<Gdk::GLContext> m_context;
  std::unique_ptr<wstdisplay::Device> m_device;

public:
  GLDevice(GLDevice const&) = delete;
  GLDevice& operator=(GLDevice const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
