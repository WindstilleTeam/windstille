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

#ifndef HEADER_SHADER_PROGRAM_HPP
#define HEADER_SHADER_PROGRAM_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include "sharedptr.hpp"

class ShaderProgramImpl;
class ShaderObject;

/** */
class ShaderProgram
{
private:
public:
  ShaderProgram();
  ~ShaderProgram();

  void attach(const ShaderObject& obj);
  void link();

  GLint get_uniform_location(const char* name);
  GLhandleARB get_handle() const;

  // FIXME: All these only work when you call
  // glUseProgramObjectARB(shader_program.get_handle()); before them
  void set_uniform1f(const char* name, GLfloat v0);
  void set_uniform2f(const char* name, GLfloat v0, GLfloat v1);
  void set_uniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2);
  void set_uniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

  void set_uniform1i(const char* name, GLint v0);
  void set_uniform2i(const char* name, GLint v0, GLint v1);
  void set_uniform3i(const char* name, GLint v0, GLint v1, GLint v2);
  void set_uniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3);

private:
  SharedPtr<ShaderProgramImpl> impl;
};

#endif

/* EOF */
