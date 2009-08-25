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

#include "util/util.hpp"

/** Read a file, user must free() the returned pointer */
static GLchar* load_file(const char* filename)
{
  size_t block_size = 4096;
  size_t size = block_size;
  size_t true_size = 0;
  GLchar* str = (char*)(malloc(size));
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
          str = (GLchar*)realloc(str, size);
        }
      str = (GLchar*)realloc(str, true_size+1);
      str[true_size] = '\0';
    }

  fclose(f);
  return str;
}

class ShaderObjectImpl
{
public:
  GLhandleARB handle;

  ShaderObjectImpl(GLenum type)
    : handle(glCreateShaderObjectARB(type))
  {
  }

  ~ShaderObjectImpl()
  {
    glDeleteObjectARB(handle);
  }
};

ShaderObject::ShaderObject(GLenum type, const std::string& filename)
  : impl(new ShaderObjectImpl(type))
{
  load(filename);
  compile();
  print_log();
}

ShaderObject::~ShaderObject()
{
}

void
ShaderObject::load(const std::string& filename)
{
  GLchar* buf = load_file(filename.c_str());
  glShaderSourceARB(impl->handle, 1, const_cast<const GLchar**>(&buf), NULL);
  assert_gl("load_source");

  std::cout << "Source:\n" << buf << std::endl;
  free(buf);
}

GLhandleARB 
ShaderObject::get_handle() const
{
  return impl->handle;
}

void
ShaderObject::compile()
{
  glCompileShaderARB(impl->handle);
}

void
ShaderObject::print_log()
{
  int infologLength = 0;
  int charsWritten  = 0;
  GLcharARB *infoLog;

  assert_gl("print_log1");

  glGetObjectParameterivARB(impl->handle, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                            &infologLength);
  assert_gl("print_log2");

  if (infologLength > 0)
    {
      infoLog = (GLcharARB*)malloc(infologLength);
      if (infoLog == NULL)
        {
          printf("ERROR: Could not allocate InfoLog buffer\n");
          exit(1);
        }
      glGetInfoLogARB(impl->handle, infologLength, &charsWritten, infoLog);
      printf("InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
    }
  assert_gl("print_log3");
}

/* EOF */
