/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_DISPLAY_OPENGL_STATE_HPP
#define HEADER_DISPLAY_OPENGL_STATE_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>

class Framebuffer;
class Texture;
class Color;
class OpenGLStateImpl;

/**
 * This class manages the state of a OpenGL context, meaning it tries
 * to reduce state changes and in addition to that it gives a well
 * default defined state to begin with. It is *not* meant to replace
 * the complete state managment of OpenGL, but limited to those states
 * that are important to a single drawing operation, such as color,
 * line width, blend mode, texture and vertex arrays. It does not
 * handle things like framebuffer objects, since those are mainly
 * usefull for more then a single operation.
 *
 * You use OpenGLState by first creating a object, then setting all
 * those states you want and finally calling activate(), so activate
 * the set state. Multiple calls to activate() are allowed.
 */
class OpenGLState
{
private:
  // pointer to the global OpenGLState, ie. that that is currently
  // used by the hardware
  static OpenGLState* global_;
  static OpenGLState* global();

public:
  /**
   * Init the global OpenGLState, must be called exactly once 
   */
  static void init();

  /** 
   * Creates a new OpenGLState with default settings, which are:
   *
   * - no texture is set
   * - vertex array use is disabled
   * - color is white, fully opaque
   * - blending is enabled with (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
   * - depth test is disabled
   */
  OpenGLState();
  ~OpenGLState();

  /** 
   * Binds the given \a texture to the given texture \a unit and
   * enables texturing via glEnable(GL_TEXTURE2D) for the unit.
   */
  void bind_texture(const Texture& texture, int unit = 0);

  void set_blend_func(GLenum sfactor, GLenum dfactor);

  void enable(GLenum cap);
  void disable(GLenum cap);

  void set_state(GLenum array, bool value);
  bool get_state(GLenum array) const;
  
  void enable_client_state(GLenum array);
  void disable_client_state(GLenum array);

  void set_client_state(GLenum array, bool value);
  bool get_client_state(GLenum array) const;

  // FIXME: Color is a per vertex attribute, doesn't make much sense
  // to have it here
  void color(const Color& color);

  /**
   * Activates the given state, you *must* call this before you issue
   * gl commands that depend on the given state 
   */
  void activate();
  
  /**
   * Verify that all states got set as planed, for debugging purpose only
   */
  void verify();
private:
  std::auto_ptr<OpenGLStateImpl> impl;

  OpenGLState (const OpenGLState&);
  OpenGLState& operator= (const OpenGLState&);
};

#endif

/* EOF */