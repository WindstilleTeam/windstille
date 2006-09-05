#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <string>
#include <stdexcept>
#include <sstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

std::string dirname(const std::string& filename);
std::string basename(const std::string& filename);
void file_to_string(const std::string& filename, std::string& str);

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
#endif
}

#endif

