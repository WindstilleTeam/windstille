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

#include "display/opengl_state.hpp"

#include <iostream>

#include "display/texture.hpp"
#include "display/color.hpp"
#include "display/assert_gl.hpp"

#define MAX_TEXTURE_UNITS 4

class OpenGLStateImpl
{
public:
  /** Used to check if the OpenGLState was properly used or if
      somebody forget the final activate() call */
  bool was_activated;

  Texture     texture[MAX_TEXTURE_UNITS];

  Color color;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

  /** glEnable/glDisable */
  std::map<GLenum, bool> state;

  /** glEnableClientState/glDisableClientState */
  std::map<GLenum, bool> client_state;

  OpenGLStateImpl()
    : was_activated(false),
      color(),
      blend_sfactor(GL_SRC_ALPHA),
      blend_dfactor(GL_ONE_MINUS_SRC_ALPHA),
      state(),
      client_state()
  {}
};

OpenGLState* OpenGLState::global_ = 0;

void
OpenGLState::init()
{
  assert(global_ == 0);

  // Init the default settings
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  global_ = new OpenGLState();

#if 0
  if (debug)
    global_->verify();
#endif
}

OpenGLState*
OpenGLState::global()
{
  return global_;
}

// The code here is just as placeholder for the moment, should be
// replaced with something more optimized later
OpenGLState::OpenGLState() :
  impl(new OpenGLStateImpl())
{
  impl->state[GL_DEPTH_TEST]  = false;
  impl->state[GL_BLEND]       = false;
  impl->state[GL_LINE_SMOOTH] = false;

  impl->client_state[GL_VERTEX_ARRAY]        = false;
  impl->client_state[GL_COLOR_ARRAY]         = false;
  impl->client_state[GL_TEXTURE_COORD_ARRAY] = false;
  impl->client_state[GL_NORMAL_ARRAY]        = false;
  
  impl->blend_sfactor      = GL_SRC_ALPHA;
  impl->blend_dfactor      = GL_ONE_MINUS_SRC_ALPHA;
  
  impl->color              = Color(1.0f, 1.0f, 1.0f);
}

OpenGLState::~OpenGLState()
{
  assert(impl->was_activated);
}

void
OpenGLState::bind_texture(const Texture& texture, int unit)
{
  assert(unit >= 0 && unit < MAX_TEXTURE_UNITS);
  impl->texture[unit] = texture;
}

void
OpenGLState::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  impl->blend_sfactor = sfactor;
  impl->blend_dfactor = dfactor;
}

bool
OpenGLState::get_state(GLenum cap) const
{
  std::map<GLenum, bool>::const_iterator i = impl->state.find(cap);
  if (i == impl->state.end())
  {
    std::cout << "OpenGLState: Unknown state requested: " << cap << std::endl;
    return false;
  }
  else
  {
    return i->second;
  }
}

void 
OpenGLState::set_state(GLenum cap, bool value)
{
  std::map<GLenum, bool>::iterator i = impl->state.find(cap);
  if (i == impl->state.end())
  {
    std::cout << "OpenGLState: Unknown state set: " << cap << std::endl;
  }
  else
  {
    i->second = value;
  } 
}

void
OpenGLState::enable(GLenum cap)
{
  // Texturing is enabled automatically when a texture is bind
  assert(cap != GL_TEXTURE_2D);

  set_state(cap, true);
}

void
OpenGLState::disable(GLenum cap)
{  
  // Texturing is enabled automatically when a texture is bind
  assert(cap != GL_TEXTURE_2D);

  set_state(cap, false);
}

bool
OpenGLState::get_client_state(GLenum array) const
{
  std::map<GLenum, bool>::const_iterator i = impl->client_state.find(array);
  if (i == impl->client_state.end())
  {
    std::cout << "OpenGLState: Unknown client state requested: " << array << std::endl;
    return false;
  }
  else
  {
    return i->second;
  }
}

void
OpenGLState::set_client_state(GLenum array, bool value)
{
  std::map<GLenum, bool>::iterator i = impl->client_state.find(array);
  if (i == impl->client_state.end())
  {
    std::cout << "OpenGLState: Unknown client state set: " << array << std::endl;
  }
  else
  {
    i->second = value;
  }
}
  
void
OpenGLState::enable_client_state(GLenum array)
{
  set_client_state(array, true);
}

void
OpenGLState::disable_client_state(GLenum array)
{
  set_client_state(array, false);
}

void
OpenGLState::color(const Color& color_)
{
  impl->color = color_;
}

void
OpenGLState::activate()
{
  // do nothing for now, should be implemented later on
  impl->was_activated = true;

  OpenGLState* global_state = OpenGLState::global();
  assert(global_state);

  // always apply color since it might have got changed between a glBegin/glEnd
  glColor4f(impl->color.r, impl->color.g, impl->color.b, impl->color.a);

  for(std::map<GLenum, bool>::iterator i = impl->state.begin();
      i != impl->state.end(); ++i)
  {
    if (global_state->get_state(i->first) != i->second)
    {
      if (i->second)
        glEnable(i->first);
      else
        glDisable(i->first);

      global_state->set_state(i->first, i->second);
    }
  }

  for(std::map<GLenum, bool>::iterator i = impl->client_state.begin();
      i != impl->client_state.end(); ++i)
  {
    if (global_state->get_client_state(i->first) != i->second)
    {
      if (i->second)
        glEnableClientState(i->first);
      else
        glDisableClientState(i->first);

      global_state->set_client_state(i->first, i->second);
    }
  }

  if (impl->blend_sfactor != global_state->impl->blend_sfactor ||
      impl->blend_dfactor != global_state->impl->blend_dfactor)
  {
    glBlendFunc(impl->blend_sfactor, impl->blend_dfactor);

    global_state->impl->blend_sfactor = impl->blend_sfactor;
    global_state->impl->blend_dfactor = impl->blend_dfactor;
  }

  for(int i = 0; i < MAX_TEXTURE_UNITS; ++i)
  {
    if (impl->texture[i] != global_state->impl->texture[i])
    { 
      // FIXME: glActiveTexture() makes the game crash on Matrox
      // G450, without that line it works
      glActiveTexture(GL_TEXTURE0 + i);
      if (impl->texture[i])
      {
        global_state->impl->texture[i] = impl->texture[i];

        switch (impl->texture[i].get_target())
        {                 
          case GL_TEXTURE_2D:
            glBindTexture(GL_TEXTURE_2D, impl->texture[i].get_handle());
            glEnable(GL_TEXTURE_2D);
            break;
                  
          default:
            assert(!"Unknown texture target");
            break;
        }
      }
      else
      {
        // FIXME: Hacky, should disable only the right target
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        global_state->impl->texture[i] = impl->texture[i];
      }
    }
  }

#if 0
  if (debug)
    verify();
#endif
}

void
OpenGLState::verify()
{
  for(std::map<GLenum, bool>::iterator i = impl->client_state.begin();
      i != impl->client_state.end(); ++i)
  {
    if (glIsEnabled(i->first) != i->second)
    {
      std::cout << "OpenGLState: client_state " << i->first << " is out of sync" << std::endl;
    }
  }  

  for(std::map<GLenum, bool>::iterator i = impl->state.begin();
      i != impl->state.end(); ++i)
  {
    if (glIsEnabled(i->first) != i->second)
    {
      std::cout << "OpenGLState: state " << i->first << " is out of sync" << std::endl;
    }
  }

  GLint sfactor; glGetIntegerv(GL_BLEND_SRC, &sfactor);
  if (sfactor != int(impl->blend_sfactor))
  {
    std::cout << "OpenGLState: src blendfunc is out of sync" << std::endl;
  }

  GLint dfactor; glGetIntegerv(GL_BLEND_DST, &dfactor);
  if (dfactor != int(impl->blend_dfactor))
  {
    std::cout << "OpenGLState: dst blendfunc is out of sync" << std::endl;
  }

  if (0)
  {
    // FIXME: Add multitexture support here
    GLint texture_handle;
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &texture_handle);
    if (impl->texture[0] && static_cast<GLuint>(texture_handle) != impl->texture[0].get_handle())
    {
      std::cout << "OpenGLState: texture handle is out of sync: " << impl->texture[0].get_handle() << std::endl;
    }
  }
  assert_gl("OpenGLState::verify");
}

/* EOF */
