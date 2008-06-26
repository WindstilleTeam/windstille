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
#endif
}

const std::wstring string_to_wstring(const std::string& str);

#endif

