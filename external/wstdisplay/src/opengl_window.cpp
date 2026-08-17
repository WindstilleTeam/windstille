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

#include "opengl_window.hpp"

#include <iostream>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "opengl_state.hpp"
#include "graphics_context.hpp"
#include "assert_gl.hpp"

namespace {

std::string get_gl_string(GLenum name)
{
  const GLubyte* ret = glGetString(name);
  if (ret == nullptr) {
    return "(null)";
  } else {
    return reinterpret_cast<char const*>(ret);
  }
}

} // namespace

namespace wstdisplay {

OpenGLWindow::OpenGLWindow(wstsys::System& system,
                           Params const& params) :
  sig_resized(),
  m_system(system),
  m_window(nullptr),
  m_gl_context(nullptr),
  m_size(params.size),
  m_mode(params.mode),
  m_resizable(params.resizable),
  m_anti_aliasing(params.anti_aliasing),
  m_gc()
{
  assert(m_window == nullptr);

  //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // FIXME: Should make this configurable, as Matrox G450 can't do it,
  // but works 'fine' without it
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  if (m_anti_aliasing != 0)
  {
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ); // boolean value, either it's enabled or not
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, m_anti_aliasing ); // 0, 2, or 4 for number of samples
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  uint32_t flags = SDL_WINDOW_OPENGL;

  if (m_mode == Mode::Fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  } else if (m_mode == Mode::FullscreenDesktop) {
    flags |= SDL_WINDOW_FULLSCREEN;
  }

  if (m_resizable) {
    flags |= SDL_WINDOW_RESIZABLE;
  }

  m_window = SDL_CreateWindow(params.title.c_str(),
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              m_size.width(), m_size.height(),
                              flags);
  if (m_window == nullptr) {
    throw std::runtime_error("Display:: Couldn't create window");
  }

  m_gl_context = SDL_GL_CreateContext(m_window);
  if (m_gl_context == nullptr) {
    throw std::runtime_error("Display:: failed to create GLContext");
  }

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::ostringstream msg;
    msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
    throw std::runtime_error(msg.str());
  }

  std::cout << "glewInit() successfull: " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "OpenGL 3.2: " << GL_VERSION_3_2 << std::endl;
  std::cout << "GL_VERSION_3_0: " << GL_VERSION_3_0 << std::endl;
  std::cout << "GL_VENDOR: " << get_gl_string(GL_VENDOR) << std::endl;
  std::cout << "GL_RENDERER: " << get_gl_string(GL_RENDERER) << std::endl;
  std::cout << "GL_VERSION: " << get_gl_string(GL_VERSION) << std::endl;
  std::cout << "GL_SHADING_LANGUAGE_VERSION: " << get_gl_string(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  // std::cout << "GL_EXTENSIONS: " << get_gl_string(GL_EXTENSIONS) << std::endl;

  if (m_anti_aliasing) {
    glEnable(GL_MULTISAMPLE);
  }

  assert_gl();

  OpenGLState::init();
  glViewport(0, 0, m_size.width(), m_size.height());

  m_gc = std::make_unique<GraphicsContext>();
  m_gc->set_ortho(m_size);

  if ((false)) // disabled for the moment, as it seems to do more harm then good
  { // Magic pixel center constant, without that textures drawn in
    // pixel coordinates might end up blurry
    m_gc->translate(0.375f, 0.375f, 0.0f);
  }

  assert_gl();
}

OpenGLWindow::~OpenGLWindow()
{
  if (m_gl_context != nullptr) {
    SDL_GL_DeleteContext(m_gl_context);
  }

  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
  }
}

void
OpenGLWindow::set_title(std::string const& title)
{
  if (m_window != nullptr) {
    SDL_SetWindowTitle(m_window, title.c_str());
  }
}

void
OpenGLWindow::set_icon(std::filesystem::path const& filename)
{
  assert(m_window != nullptr);

  surf::SoftwareSurface const pixeldata = surf::SoftwareSurface::from_file(filename);

  SDL_Surface* sdl_surface;
  if constexpr (std::endian::native == std::endian::big) {
    sdl_surface = SDL_CreateRGBSurfaceFrom(const_cast<uint8_t*>(static_cast<uint8_t const*>(pixeldata.get_data())),
                                           pixeldata.get_width(),
                                           pixeldata.get_height(),
                                           pixeldata.get_format() == surf::PixelFormat::RGB8 ? 24 : 32,
                                           pixeldata.get_pitch(),
                                           0xff000000,
                                           0x00ff0000,
                                           0x0000ff00,
                                           pixeldata.get_format() == surf::PixelFormat::RGB8 ? 0x00000000 : 0x000000ff);
  } else {
    sdl_surface = SDL_CreateRGBSurfaceFrom(const_cast<uint8_t*>(static_cast<uint8_t const*>(pixeldata.get_data())),
                                           pixeldata.get_width(),
                                           pixeldata.get_height(),
                                           pixeldata.get_format() == surf::PixelFormat::RGB8 ? 24 : 32,
                                           pixeldata.get_pitch(),
                                           0x000000ff,
                                           0x0000ff00,
                                           0x00ff0000,
                                           pixeldata.get_format() == surf::PixelFormat::RGB8 ? 0x00000000 : 0xff000000);
  }

  SDL_SetWindowIcon(m_window, sdl_surface);

  SDL_FreeSurface(sdl_surface);
}

void
OpenGLWindow::set_size(geom::isize const& size)
{
  m_size = size;
}

geom::isize
OpenGLWindow::get_size() const
{
  if (m_window != nullptr) {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return geom::isize(w, h);
  } else {
    return m_size;
  }
}

void
OpenGLWindow::set_mode(Mode mode)
{
  m_mode = mode;

  if (m_window != nullptr) {
    switch (m_mode)
    {
      case Mode::FullscreenDesktop:
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        break;

      case Mode::Fullscreen:
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
        break;

      case Mode::Window:
        SDL_SetWindowFullscreen(m_window, 0);
        break;
    }
  }
}

OpenGLWindow::Mode
OpenGLWindow::get_mode() const
{
  return m_mode;
}

void
OpenGLWindow::set_gamma(float r, float g, float b)
{
  if (m_window != nullptr) {
    if (SDL_SetWindowBrightness(m_window, (r+g+b)/3.0f) == -1)
    {
      // Couldn't set gamma
    }
  }
}

GraphicsContext&
OpenGLWindow::get_gc() const
{
  assert(m_gc != nullptr);

  return *m_gc;
}

void
OpenGLWindow::swap_buffers()
{
  assert(m_window != nullptr);

  SDL_GL_SwapWindow(m_window);
}

surf::SoftwareSurface
OpenGLWindow::screenshot() const
{
  assert(m_gc != nullptr);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  geom::isize size(viewport[2] / 4 * 4, viewport[3]);
  surf::SoftwareSurface surface = surf::SoftwareSurface::create(surf::PixelFormat::RGB8, size);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, surface.get_width(), surface.get_height(), GL_RGB, GL_UNSIGNED_BYTE, surface.get_data());
  assert_gl();

  return surface;
}

uint32_t
OpenGLWindow::get_id() const
{
  return SDL_GetWindowID(m_window);
}

void
OpenGLWindow::handle_event(SDL_WindowEvent window)
{
  switch (window.event)
  {
    case SDL_WINDOWEVENT_SHOWN:
      break;

    case SDL_WINDOWEVENT_HIDDEN:
      break;

    case SDL_WINDOWEVENT_EXPOSED:
      break;

    case SDL_WINDOWEVENT_MOVED:
      break;

    case SDL_WINDOWEVENT_RESIZED:
      m_size = geom::isize(window.data1, window.data2);
      glViewport(0, 0, m_size.width(), m_size.height());
      sig_resized(m_size);
      break;

    case SDL_WINDOWEVENT_SIZE_CHANGED:
      break;

    case SDL_WINDOWEVENT_MINIMIZED:
      break;

    case SDL_WINDOWEVENT_MAXIMIZED:
      break;

    case SDL_WINDOWEVENT_RESTORED:
      break;

    case SDL_WINDOWEVENT_ENTER:
      break;

    case SDL_WINDOWEVENT_LEAVE:
      break;

    case SDL_WINDOWEVENT_FOCUS_GAINED:
      break;

    case SDL_WINDOWEVENT_FOCUS_LOST:
      break;

    case SDL_WINDOWEVENT_CLOSE:
      break;

    case SDL_WINDOWEVENT_TAKE_FOCUS:
      break;

    case SDL_WINDOWEVENT_HIT_TEST:
      break;
  }
}

} // namespace wstdisplay

/* EOF */
