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

#include "display/surface.hpp"

#include "display/opengl_state.hpp"
#include "math/quad.hpp"
#include "display/surface_drawing_parameters.hpp"
#include "display/surface_manager.hpp"

SurfacePtr
Surface::create(const Pathname& filename)
{
  return SurfaceManager::current()->get(filename);
}

SurfacePtr
Surface::create(TexturePtr texture, const Rectf& uv, const Sizef& size)
{
  return SurfacePtr(new Surface(texture, uv, size));
}

SurfacePtr
Surface::create(int width, int height)
{
  return SurfacePtr(new Surface(width, height));
}

Surface::Surface(int width, int height) :
  m_texture(),
  m_uv(),
  m_size()
{
  m_size  = Size(width, height);

  m_texture = Texture::create(GL_TEXTURE_2D, width, height);
  m_uv      = Rectf(0.0f, 0.0f,
                    m_size.width  / static_cast<float>(m_texture->get_width()),
                    m_size.height / static_cast<float>(m_texture->get_height()));
}

Surface::Surface(TexturePtr texture, const Rectf& uv, const Sizef& size) :
  m_texture(),
  m_uv(),
  m_size()
{
  m_texture = texture;
  m_uv      = uv;
  m_size    = size;
}

Surface::~Surface()
{
}

float
Surface::get_width()  const
{
  return m_size.width;
}

float
Surface::get_height() const
{ 
  return m_size.height; 
}

Sizef
Surface::get_size() const
{
  return m_size;
}

TexturePtr
Surface::get_texture() const
{
  return m_texture;
}

Rectf
Surface::get_uv() const
{
  return m_uv;
}

void
Surface::draw(const Vector2f& pos) const
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.bind_texture(m_texture);
  state.activate();

  glBegin(GL_QUADS);

  glTexCoord2f(m_uv.left, m_uv.top);
  glVertex2f(pos.x, pos.y);

  glTexCoord2f(m_uv.right, m_uv.top);
  glVertex2f(pos.x + m_size.width, pos.y);

  glTexCoord2f(m_uv.right, m_uv.bottom);
  glVertex2f(pos.x + m_size.width, pos.y + m_size.height);

  glTexCoord2f(m_uv.left, m_uv.bottom);
  glVertex2f(pos.x, pos.y + m_size.height);

  glEnd();
}

void
Surface::draw(const SurfaceDrawingParameters& params) const
{
  OpenGLState state;
  state.enable(GL_BLEND);
  state.set_blend_func(params.blendfunc_src, params.blendfunc_dst);
  state.bind_texture(m_texture);
  state.color(params.color);
  state.activate();

  glBegin(GL_QUADS);

  Rectf uv = m_uv;

  if (params.hflip)
    std::swap(uv.left, uv.right);

  if (params.vflip)
    std::swap(uv.top, uv.bottom);

  Quad quad(params.pos.x, 
            params.pos.y,
            params.pos.x + m_size.width  * params.scale.x, 
            params.pos.y + m_size.height * params.scale.y);

  quad.rotate(params.angle);
  
  glTexCoord2f(uv.left, uv.top);
  glVertex3f(quad.p1.x, quad.p1.y, params.z_pos);

  glTexCoord2f(uv.right, uv.top);
  glVertex3f(quad.p2.x, quad.p2.y, params.z_pos);

  glTexCoord2f(uv.right, uv.bottom);
  glVertex3f(quad.p3.x, quad.p3.y, params.z_pos);

  glTexCoord2f(uv.left, uv.bottom);
  glVertex3f(quad.p4.x, quad.p4.y, params.z_pos);

  glEnd(); 
}

/* EOF */
