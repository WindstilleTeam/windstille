/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP
#define HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include <GL/glew.h>

class ShaderObject;
typedef std::shared_ptr<ShaderObject> ShaderObjectPtr;

class ShaderObject
{
public:
  static ShaderObjectPtr from_file(GLenum type, std::filesystem::path const& filename);
  static ShaderObjectPtr from_string(GLenum type, std::string_view source);

private:
  ShaderObject(GLenum type);

public:
  ~ShaderObject();

  std::string get_info_log() const;
  bool get_compile_status() const;

  GLuint get_handle() const;
  void load(std::string_view source);
  void compile();
  void print_log() const;

private:
  GLuint m_handle;
};

#endif

/* EOF */
