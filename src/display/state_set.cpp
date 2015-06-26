/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "state_set.hpp"

StateSet::StateSet()
{
}

void
StateSet::set_texture(TexturePtr texture, int unit)
{
}

void
StateSet::set_blend_func(GLenum sfactor, GLenum dfactor)
{
}

void
StateSet::set_program(ShaderProgramPtr prog)
{
}

void
StateSet::set_uniform1f(const char* name, GLfloat v0)
{
}

void
StateSet::set_uniform2f(const char* name, GLfloat v0, GLfloat v1)
{
}

void
StateSet::set_uniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2)
{
}

void
StateSet::set_uniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
}

void
StateSet::set_uniform1i(const char* name, GLint v0)
{
}

void
StateSet::set_uniform2i(const char* name, GLint v0, GLint v1)
{
}

void
StateSet::set_uniform3i(const char* name, GLint v0, GLint v1, GLint v2)
{
}

void
StateSet::set_uniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3)
{
}

/* EOF */
