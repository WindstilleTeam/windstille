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

#include <iostream>

#include "surface.hpp"
#include "math/vector2f.hpp"
#include "math/quad.hpp"
#include "display/opengl_state.hpp"
#include "surface_drawing_parameters.hpp"
#include "surface_manager.hpp"

class SurfaceImpl
{
public:
  /**
   * Texture on which the surface is located
   */
  Texture texture;

  /** 
   * uv coordinates of the Surface in [0,1] range
   */
  Rectf   uv;

  /**
   * The size of the Surface in pixels
   */
  Size size;
};

Surface::Surface()
{
}

Surface::Surface(const std::string& filename)
{
  // FIXME: a bit ugly, should move some of the surface_manager code over here
  *this = SurfaceManager::current()->get(filename);
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

Surface::Surface(int width, int height)
  : impl(new SurfaceImpl())
{
  impl->size  = Size(width, height);

  impl->texture = Texture(GL_TEXTURE_2D, power_of_two(width), power_of_two(height));
  impl->uv      = Rectf(0, 0,
                        float(impl->size.width)  / impl->texture.get_width(),
                        float(impl->size.height) / impl->texture.get_height());
}

Surface::Surface(Texture texture, const Rectf& rect, int width, int height)
  : impl(new SurfaceImpl())
{
  impl->texture = texture;
  impl->size.width   = width;
  impl->size.height  = height;
  impl->uv      = rect;
}

Surface::~Surface()
{
}

int
Surface::get_width()  const
{
  return impl->size.width;
}

int
Surface::get_height() const
{ 
  return impl->size.height; 
}

Texture
Surface::get_texture() const
{
  return impl->texture;
}

Rectf
Surface::get_uv() const
{
  return impl->uv;
}

Surface::operator bool() const
{
  return impl.get() != 0;
}

void
Surface::draw(const Vector2f& pos) const
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.bind_texture(impl->texture);
  state.activate();

  glBegin(GL_QUADS);

  glTexCoord2f(impl->uv.left, impl->uv.top);
  glVertex2f(pos.x, pos.y);

  glTexCoord2f(impl->uv.right, impl->uv.top);
  glVertex2f(pos.x + impl->size.width, pos.y);

  glTexCoord2f(impl->uv.right, impl->uv.bottom);
  glVertex2f(pos.x + impl->size.width, pos.y + impl->size.height);

  glTexCoord2f(impl->uv.left, impl->uv.bottom);
  glVertex2f(pos.x, pos.y + impl->size.height);

  glEnd();
}

void
Surface::draw(const SurfaceDrawingParameters& params) const
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(params.blendfunc_src, params.blendfunc_dst);
  state.bind_texture(impl->texture);
  state.color(params.color);
  state.activate();

  glBegin(GL_QUADS);

  Quad quad(params.pos.x, 
            params.pos.y,
            params.pos.x + impl->size.width  * params.scale.x, 
            params.pos.y + impl->size.height * params.scale.y);

  quad.rotate(params.angle);
  
  glTexCoord2f(impl->uv.left, impl->uv.top);
  glVertex2f(quad.p1.x, quad.p1.y);

  glTexCoord2f(impl->uv.right, impl->uv.top);
  glVertex2f(quad.p2.x, quad.p2.y);

  glTexCoord2f(impl->uv.right, impl->uv.bottom);
  glVertex2f(quad.p3.x, quad.p3.y);

  glTexCoord2f(impl->uv.left, impl->uv.bottom);
  glVertex2f(quad.p4.x, quad.p4.y);

  glEnd(); 
}

/* EOF */
