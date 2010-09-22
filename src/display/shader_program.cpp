/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/shader_object.hpp"
#include "display/assert_gl.hpp"
#include "display/shader_program.hpp"

ShaderProgramPtr
ShaderProgram::create()
{
  return ShaderProgramPtr(new ShaderProgram());
}

ShaderProgram::ShaderProgram() :
 m_handle(0)
{
  m_handle = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(m_handle);
}

void
ShaderProgram::attach(ShaderObjectPtr obj)
{
  glAttachShader(m_handle, obj->get_handle());
}

void
ShaderProgram::link()
{
  glLinkProgram(m_handle);
}

GLint
ShaderProgram::get_uniform_location(const char* name)
{
  GLint loc;

  loc = glGetUniformLocation(m_handle, name);

  if (loc == -1)
    std::cout << "No such uniform named \"%s\"\n" << name << std::endl;

  assert_gl("ShaderProgram::get_uniform_location()");

  return loc;
}

GLuint 
ShaderProgram::get_handle() const
{
  return m_handle;
}

void
ShaderProgram::set_uniform1f(const char* name, GLfloat v0)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform1f(location, v0);
}

void
ShaderProgram::set_uniform2f(const char* name, GLfloat v0, GLfloat v1)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform2f(location, v0, v1);
}

void
ShaderProgram::set_uniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform3f(location, v0, v1, v2);
}

void
ShaderProgram::set_uniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform4f(location, v0, v1, v2, v3);
}

void
ShaderProgram::set_uniform1i(const char* name, GLint v0)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform1i(location, v0);
}

void
ShaderProgram::set_uniform2i(const char* name, GLint v0, GLint v1)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform2i(location, v0, v1);
}

void
ShaderProgram::set_uniform3i(const char* name, GLint v0, GLint v1, GLint v2)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform3i(location, v0, v1, v2);
}

void
ShaderProgram::set_uniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3)
{
  GLint location = glGetUniformLocation(m_handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform4i(location, v0, v1, v2, v3);
}

/* EOF */
