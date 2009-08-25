/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_UTIL_UTIL_HPP
#define HEADER_WINDSTILLE_UTIL_UTIL_HPP

#include <sstream>

std::string dirname(const std::string& filename);
std::string basename(const std::string& filename);

/** Loads a file from \a filename and places its content in \a str
    FIXME: doesn't use PhysFS */
void file_to_string(const std::string& filename, std::string& str);

bool has_suffix(const std::string& str, const std::string& suffix);
std::string tolowercase(const std::string& str);

template<class T>
std::string to_string(const T& data)
{
  std::ostringstream os;
  os << data;
  return os.str();
}

static inline void assert_gl(const char* message)
{
#ifdef DEBUG
  GLenum error = glGetError();
  if(error != GL_NO_ERROR) {
    std::ostringstream msg;
    msg << "OpenGLError while '" << message << "': "
        << gluErrorString(error);
    throw std::runtime_error(msg.str());
  }
#else
  // Stop compiler from issueing 'unused parameter'
  do { (void)(message); } while (0);
#endif
}

#endif

/* EOF */
