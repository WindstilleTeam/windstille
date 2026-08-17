// Windstille Display Library
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// Shared OpenGL / OpenGL ES 2.0 includes.  Pattern follows Pingus
// opengl_compat.hpp — keep desktop and embedded paths in one place.

#ifndef HEADER_WSTDISPLAY_GL_COMPAT_HPP
#define HEADER_WSTDISPLAY_GL_COMPAT_HPP

#ifdef _WIN32
#  define NOGDI
#endif

#if defined(WSTDISPLAY_USE_GLES) || defined(WSTDISPLAY_EMSCRIPTEN)

#  include <SDL_opengles2.h>
#  ifndef WSTDISPLAY_GL_ES
#    define WSTDISPLAY_GL_ES 1
#  endif
/* GLES2 does not define GL_QUADS; keep a sentinel for asserts only. */
#  ifndef GL_QUADS
#    define GL_QUADS 0x0007
#  endif
/* Map common desktop internal formats to GLES2-friendly ones. */
#  ifndef GL_RGB8
#    define GL_RGB8 GL_RGB
#  endif
#  ifndef GL_RGBA8
#    define GL_RGBA8 GL_RGBA
#  endif
/* HDR float colour buffers need EXT_color_buffer_float; fall back to RGBA. */
#  ifndef GL_RGBA16F
#    define GL_RGBA16F GL_RGBA
#  endif
#  ifndef GL_DEPTH24_STENCIL8
#    define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#  endif
/* If OES packed depth/stencil is absent, depth-only is still better than failing compile. */
#  ifndef GL_DEPTH24_STENCIL8_OES
#    undef GL_DEPTH24_STENCIL8
#    define GL_DEPTH24_STENCIL8 GL_DEPTH_COMPONENT16
#  endif

#else

// Desktop OpenGL via GLEW (must be included before gl.h / SDL_opengl.h)
#  include <GL/glew.h>
#  ifndef WSTDISPLAY_GL_ES
#    define WSTDISPLAY_GL_ES 0
#  endif

#endif

#endif

// EOF //
