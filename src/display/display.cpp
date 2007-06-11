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
#include <config.h>

#include <png.h>
#include <math.h>
#include <errno.h>
#include <fstream>
#include <stdexcept>
#include <SDL.h>
#include "console.hpp"
#include "config.hpp"
#include "display/opengl_state.hpp"
#include "display.hpp"
#include "util.hpp"
#include <assert.h>

SDL_Surface* Display::window       = 0;
std::vector<Rect> Display::cliprects;
std::vector<Framebuffer> framebuffers;

void
Display::draw_line(const Vector& pos1, const Vector& pos2, const Color& color)
{
  OpenGLState state;

  state.enable(GL_LINE_SMOOTH);
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINES);
  glVertex2f(pos1.x, pos1.y);
  glVertex2f(pos2.x, pos2.y);
  glEnd(); 
}

void
Display::fill_rect(const Rectf& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_QUADS);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glEnd();
}

void
Display::draw_rect(const Rectf& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINE_STRIP);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glVertex2f(rect.left,  rect.top);
  glEnd();
}

void
Display::fill_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);


  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 8;
  glBegin(GL_QUAD_STRIP);
  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.top - y);
      glVertex2f(irect.right + x, irect.top - y);
    }
  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.bottom + y);
      glVertex2f(irect.right + x, irect.bottom + y);
    }
  glEnd();
}

void
Display::draw_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.enable(GL_LINE_SMOOTH);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 4;
  glBegin(GL_LINE_STRIP);
  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.top - y);
    }
  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.bottom + y);
    }

  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.right + x, irect.bottom + y);
    }

  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;
        
      glVertex2f(irect.right + x, irect.top - y);
    }
  // go back to start
  glVertex2f(irect.left, irect.top - radius);

  glEnd();
}

int
Display::get_width()
{
  return window->w;
}

int
Display::get_height()
{
  return window->h;
}

void
Display::init()
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

  if (config.get_int("anti-aliasing"))
    {
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ); // boolean value, either it's enabled or not
      SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, config.get_int("anti-aliasing") ); // 0, 2, or 4 for number of samples
    }

  window = SDL_SetVideoMode(config.get_int("screen-width"), config.get_int("screen-height"),
                            0, SDL_OPENGL | (config.get_bool("fullscreen") ? SDL_FULLSCREEN : 0));
  if (!window)
    {
      throw std::runtime_error("Display:: Couldn't create window");
    }

  SDL_WM_SetCaption("Windstille", 0 /* icon */);

  GLenum err = glewInit();
  if(err != GLEW_OK) {
      std::ostringstream msg;
      msg << "Display:: Couldn't initialize glew: " << glewGetString(err);
      throw std::runtime_error(msg.str());
  }
  /*
  if(!GLEW_EXT_framebuffer_object) {
      std::ostringstream msg;
      msg << "Display:: Framebuffer opengl extension not supported";
      throw std::runtime_error(msg.str());
  }
  */

  glViewport(0, 0, window->w, window->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  static const float cl_pixelcenter_constant = 0.375;

  //glOrtho(0.0, window->w, window->h, 0.0, -1000.0, 1000.0);
  glOrtho(0.0, 800, 600, 0.0, 1000.0, -1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(cl_pixelcenter_constant, cl_pixelcenter_constant, 0.0);

  if (config.get_int("anti-aliasing"))
    glEnable(GL_MULTISAMPLE_ARB); 

  assert_gl("setup projection");

  OpenGLState::init();
}

void
Display::set_fullscreen(bool fullscreen)
{ 
  Uint32 flags = SDL_OPENGL;
  if (fullscreen)
    flags |= SDL_FULLSCREEN;

  window = SDL_SetVideoMode(config.get_int("screen-width"), config.get_int("screen-height"),
                            0, flags);
  if (!window)
    {
      throw std::runtime_error("Display:: Couldn't create window");
    }
}

void
Display::draw_circle(const Vector& pos, float radius, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.enable(GL_LINE_SMOOTH);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 4;
  glBegin(GL_LINE_STRIP);
  glVertex2f(radius + pos.x, pos.y);
  for(int i = 1; i < n * 4; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;
      
      glVertex2f(x + pos.x, y + pos.y);
    }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
Display::fill_circle(const Vector& pos, float radius, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 4;
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(pos.x, pos.y);
  glVertex2f(radius + pos.x, pos.y);
  for(int i = 1; i < n * 4; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;
      
      glVertex2f(x + pos.x, y + pos.y);
    }
  glVertex2f(radius + pos.x, pos.y);
  glEnd();
}

void
Display::push_cliprect(const Rect& rect_)
{
  Rect rect = rect_;

  if (!cliprects.empty())
    {
      rect.left   = std::max(rect.left, cliprects.back().left);
      rect.top    = std::max(rect.top,  cliprects.back().top);

      rect.right  = std::min(rect.right,  cliprects.back().right);
      rect.bottom = std::min(rect.bottom, cliprects.back().bottom);
    }

  cliprects.push_back(rect);

  glScissor(rect.left, get_height() - rect.top - rect.get_height(),
            rect.get_width(), rect.get_height());
  glEnable(GL_SCISSOR_TEST);
}

void
Display::pop_cliprect()
{
  assert(cliprects.size() >= 1);

  cliprects.pop_back();

  if (!cliprects.empty())
    {
      const Rect& rect = cliprects.back();

      glScissor(rect.left, get_height() - rect.top - rect.get_height(),
                rect.get_width(), rect.get_height());
    }
  else
    {
      glDisable(GL_SCISSOR_TEST);
    }
}

void
Display::set_gamma(float r, float g, float b)
{
  if (SDL_SetGamma(r, g, b) == -1)
    {
      // Couldn't set gamma
    }
}

void
Display::save_screenshot(const std::string& filename)
{
  int len = get_width() * get_height() * 3;
  GLbyte pixels[len];
  glReadPixels(0, 0, get_width(), get_height(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

  if (0)
    {
      int pitch = get_width() * 3;

      // save to ppm
      std::ofstream out(filename.c_str());
      out << "P6\n"
          << "# Windstille Screenshot\n"
          << get_width() << " " << get_height() << "\n"
          << "255\n";
      
      for(int y = get_height()-1; y >= 0; --y)
        out.write(reinterpret_cast<const char*>(pixels + y*pitch), pitch);

      out.close();
    }
  else if (0) // BMP saving
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, get_width(), get_height(), 24, get_width()*3,
                                                      0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
      SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, get_width(), get_height(), 24, get_width()*3,
                                                      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

      SDL_SaveBMP(surface, filename.c_str());
      SDL_FreeSurface(surface);
    }
  else // PNG saving
    {
      FILE* fp = fopen(filename.c_str(), "w");

      if (!fp)
        {
          console << "Error: Couldn't save screenshot: " << strerror(errno) << std::endl;
          return;
        }
      else
        {
          int pitch   = get_width() * 3;
          png_structp png_ptr;
          png_infop   info_ptr;

          png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
          info_ptr = png_create_info_struct(png_ptr);

          png_init_io(png_ptr, fp);

          png_set_IHDR(png_ptr, info_ptr, 
                       get_width(), get_height(), 8 /* bitdepth */,
                       PNG_COLOR_TYPE_RGB,
                       PNG_INTERLACE_NONE, 
                       PNG_COMPRESSION_TYPE_BASE, 
                       PNG_FILTER_TYPE_BASE);
      
          png_set_compression_level(png_ptr, 6);
          png_write_info(png_ptr, info_ptr);

          png_uint_32 height = get_height();
          png_bytep row_pointers[height];
   
          // generate row pointers
          for (unsigned int k = 0; k < height; k++)
            row_pointers[k] = reinterpret_cast<png_byte*>(pixels + ((height - k - 1) * pitch));

          png_write_image(png_ptr, row_pointers);

          png_write_end(png_ptr, info_ptr);

          fclose(fp);
        }
    }
}

void
Display::push_framebuffer(Framebuffer& framebuffer)
{
  framebuffers.push_back(framebuffer);
  // FIXME: How expensive is it to switch framebuffers? might be worth
  // to optimze some switches away
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffers.back().get_handle());
}

void
Display::pop_framebuffer()
{
  assert(!framebuffers.empty());

  framebuffers.pop_back();
  
  if (!framebuffers.empty())
    {
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffers.back().get_handle());
    }
  else
    {
          glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}

Framebuffer
Display::get_framebuffer()
{
  if (framebuffers.empty())
    return Framebuffer();
  else
    return framebuffers.back();
}

/* EOF */