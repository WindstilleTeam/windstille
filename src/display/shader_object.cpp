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

#include "display/shader_object.hpp"

#include <stdio.h>
#include <iostream>
#include <boost/scoped_array.hpp>
#include <stdexcept>

#include "display/assert_gl.hpp"

/** Read a file, user must free() the returned pointer */
static GLchar* load_file(const char* filename)
{
  size_t block_size = 4096;
  size_t size = block_size;
  size_t true_size = 0;
  GLchar* str = static_cast<char*>(malloc(size));
  FILE* f = fopen(filename, "r");
  if (!f)
  {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  else
  {
    while ((true_size += fread(str, sizeof(GLchar), 4096, f)) == 4096)
    {
      size += block_size;
      str = static_cast<GLchar*>(realloc(str, size));
    }
    str = static_cast<GLchar*>(realloc(str, true_size+1));
    str[true_size] = '\0';
  }

  fclose(f);
  return str;
}

ShaderObjectPtr
ShaderObject::create_from_file(GLenum type, const std::string& filename)
{
  return ShaderObjectPtr(new ShaderObject(type, filename));
}

ShaderObject::ShaderObject(GLenum type, const std::string& filename) :
  m_handle(0)
{
  m_handle = glCreateShader(type);

  load(filename);
  compile();
}

ShaderObject::~ShaderObject()
{
  glDeleteShader(m_handle);
}

void
ShaderObject::load(const std::string& filename)
{
  GLchar* buf = load_file(filename.c_str());
  glShaderSource(m_handle, 1, const_cast<const GLchar**>(&buf), NULL);
  assert_gl("load_source");

  //std::cout << "Source:\n" << buf << std::endl;
  free(buf);
}

GLuint 
ShaderObject::get_handle() const
{
  return m_handle;
}

void
ShaderObject::compile()
{
  glCompileShader(m_handle);
  if (!get_compile_status())
  {
    throw std::runtime_error(get_info_log());
  }
}

void
ShaderObject::print_log()
{
  int infologLength = 0;
  int charsWritten  = 0;
  GLchar* infoLog;

  assert_gl("print_log1");
  glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &infologLength);
  assert_gl("print_log2");

  if (infologLength > 0)
  {
    infoLog = static_cast<GLchar*>(malloc(infologLength));
    if (infoLog == NULL)
    {
      printf("ERROR: Could not allocate InfoLog buffer\n");
      exit(1);
    }
    glGetShaderInfoLog(m_handle, infologLength, &charsWritten, infoLog);
    printf("InfoLog:\n%s\n\n", infoLog);
    free(infoLog);
  }
  assert_gl("print_log3");
}


bool
ShaderObject::get_compile_status()
{
  int status;
  glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status);
  return (status == GL_TRUE);
}

std::string
ShaderObject::get_info_log()
{
  int info_log_len = 0;
  int charsWritten  = 0;

  glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &info_log_len);

  if (info_log_len > 0)
  {
    boost::scoped_array<GLchar> info_log(new GLchar[info_log_len]);
    glGetShaderInfoLog(m_handle, info_log_len, &charsWritten, info_log.get());
    return info_log.get();
  }
  else
  {
    return std::string("<empty>");
  }
}

/* EOF */
