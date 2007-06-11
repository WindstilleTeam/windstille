/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "shader_program.hpp"
#include <iostream>
#include "shader_object.hpp"
#include "util.hpp"

class ShaderProgramImpl
{
public:
  GLhandleARB handle;

  ShaderProgramImpl() {
    handle = glCreateProgramObjectARB();
  }

  ~ShaderProgramImpl() {
    glDeleteObjectARB(handle);
  }
};

ShaderProgram::ShaderProgram()
  : impl(new ShaderProgramImpl())
{
}

ShaderProgram::~ShaderProgram()
{
}

void
ShaderProgram::attach(const ShaderObject& obj)
{
  glAttachObjectARB(impl->handle, obj.get_handle());
}

void
ShaderProgram::link()
{
  glLinkProgramARB(impl->handle);
}

GLint
ShaderProgram::get_uniform_location(const char* name)
{
  GLint loc;

  loc = glGetUniformLocationARB(impl->handle, name);

  if (loc == -1)
    printf("No such uniform named \"%s\"\n", name);

  assert_gl("ShaderProgram::get_uniform_location()");

  return loc;
}

GLhandleARB 
ShaderProgram::get_handle() const
{
  return impl->handle;
}

void
ShaderProgram::set_uniform1f(const char* name, GLfloat v0)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform1fARB(location, v0);
}

void
ShaderProgram::set_uniform2f(const char* name, GLfloat v0, GLfloat v1)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform2fARB(location, v0, v1);
}

void
ShaderProgram::set_uniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform3fARB(location, v0, v1, v2);
}

void
ShaderProgram::set_uniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform4fARB(location, v0, v1, v2, v3);
}

void
ShaderProgram::set_uniform1i(const char* name, GLint v0)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform1iARB(location, v0);
}

void
ShaderProgram::set_uniform2i(const char* name, GLint v0, GLint v1)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform2iARB(location, v0, v1);
}

void
ShaderProgram::set_uniform3i(const char* name, GLint v0, GLint v1, GLint v2)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform3iARB(location, v0, v1, v2);
}

void
ShaderProgram::set_uniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3)
{
  GLint location = glGetUniformLocationARB(impl->handle, name);
  if (location == -1)
    std::cout << "No such uniform named \"" << name << "\"" << std::endl;
  else
    glUniform4iARB(location, v0, v1, v2, v3);
}

/* EOF */