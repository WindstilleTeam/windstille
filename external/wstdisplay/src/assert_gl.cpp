// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "assert_gl.hpp"

#include <stdexcept>
#include <sstream>
#include <GL/glew.h>

void assert_gl_loc(char const* file, int line, char const* message)
{
  GLenum error = glGetError();
  if(error != GL_NO_ERROR)
  {
    std::ostringstream msg;
    msg << file << ":" << line << ": OpenGLError while '" << (message ? message : "<null>") << "': "
        << gluErrorString(error);
    throw std::runtime_error(msg.str());
  }
}

/* EOF */
