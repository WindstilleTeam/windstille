/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_OPENGL_WINDOW_HPP
#define HEADER_WINDSTILLE_DISPLAY_OPENGL_WINDOW_HPP

#include <filesystem>
#include <memory>

#include <geom/size.hpp>

class GraphicsContext;
class OpenGLWindowImpl;

class OpenGLWindow final
{
public:
  OpenGLWindow(const std::string& title,
               const geom::isize& size, const geom::isize& aspect,
               bool fullscreen = false, int anti_aliasing = 0);
  ~OpenGLWindow();

  void set_icon(std::filesystem::path const& filename);

  int  get_width() const;
  int  get_height() const;
  geom::isize get_size() const;

  void set_fullscreen(bool fullscreen);
  void set_gamma(float r, float g, float b);

  GraphicsContext& get_gc() const;

  void swap_buffers();

private:
  std::unique_ptr<OpenGLWindowImpl> m_impl;

private:
  OpenGLWindow(const OpenGLWindow&);
  OpenGLWindow& operator=(const OpenGLWindow&);
};

#endif

/* EOF */
