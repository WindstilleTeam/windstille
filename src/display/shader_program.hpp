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

#ifndef HEADER_WINDSTILLE_DISPLAY_SHADER_PROGRAM_HPP
#define HEADER_WINDSTILLE_DISPLAY_SHADER_PROGRAM_HPP

#include <GL/glew.h>
#include <memory>
#include <string_view>

#include "display/shader_object.hpp"

class ShaderObject;
class ShaderProgram;
typedef std::shared_ptr<ShaderProgram> ShaderProgramPtr;

class ShaderProgram
{
public:
  static ShaderProgramPtr create();
  static ShaderProgramPtr from_file(std::filesystem::path const& frag_filename,
                                    std::filesystem::path const& vert_filename);
  static ShaderProgramPtr from_string(std::string_view frag_source,
                                      std::string_view vert_source);

private:
  ShaderProgram();

public:
  ~ShaderProgram();

  void attach(ShaderObjectPtr obj);
  void link();

  std::string get_info_log() const;
  bool get_link_status() const;

  GLint get_uniform_location(const char* name) const;
  GLint get_attrib_location(const char* name) const;
  GLuint get_handle() const;

  // FIXME: All these only work when you call
  // glUseProgram(shader_program.get_handle()); before them
  void set_uniform1f(const char* name, GLfloat v0);
  void set_uniform2f(const char* name, GLfloat v0, GLfloat v1);
  void set_uniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2);
  void set_uniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

  void set_uniform1i(const char* name, GLint v0);
  void set_uniform2i(const char* name, GLint v0, GLint v1);
  void set_uniform3i(const char* name, GLint v0, GLint v1, GLint v2);
  void set_uniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3);

  void bind_frag_data_location(GLuint color_number, const char* name);

private:
  GLuint m_handle;
};

#endif

/* EOF */
