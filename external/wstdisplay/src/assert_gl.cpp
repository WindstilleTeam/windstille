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
#include <iostream>
#include <cstdlib>
#include <wstdisplay/gl_compat.hpp>

namespace {

char const* gl_error_string(GLenum error)
{
  switch (error) {
    case GL_NO_ERROR:                      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
#endif
    case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
#endif
    default:                               return "unknown GL error";
  }
}

} // namespace

void assert_gl_loc(char const* file, int line, char const* message)
{
  GLenum error = glGetError();
  if(error != GL_NO_ERROR)
  {
    std::ostringstream msg;
    msg << file << ":" << line << ": OpenGLError while '"
        << (message ? message : "<null>") << "': "
        << gl_error_string(error) << " (0x" << std::hex << error << ")";
    std::cerr << msg.str() << std::endl;
    throw std::runtime_error(msg.str());
  }
}

/* EOF */
