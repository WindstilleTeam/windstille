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

#ifndef HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP
#define HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP

#include <GL/glew.h>
#include <string>
#include <boost/shared_ptr.hpp>

class ShaderObject;
typedef boost::shared_ptr<ShaderObject> ShaderObjectPtr;

class ShaderObject
{
public:
  static ShaderObjectPtr create_from_file(GLenum type, const std::string& filename);

private:
  ShaderObject(GLenum type, const std::string& filename);

public:
  ~ShaderObject();

  std::string get_info_log();
  bool get_compile_status();

  GLuint get_handle() const;
  void load(const std::string& filename);
  void compile();
  void print_log();

private:
  GLuint m_handle;
};

#endif

/* EOF */
