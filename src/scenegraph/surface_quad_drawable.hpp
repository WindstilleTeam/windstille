/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_QUAD_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_QUAD_DRAWABLE_HPP

#include "math/vector2f.hpp"
#include "math/quad.hpp"
#include "display/opengl_state.hpp"
#include "scenegraph/drawable.hpp"

class SurfaceQuadDrawable : public Drawable
{
private:
  SurfacePtr m_surface;
  Quad m_quad;
  DrawingParameters m_params;

public:
  SurfaceQuadDrawable(SurfacePtr surface, const Vector2f& pos_, const Quad& quad, 
                      const DrawingParameters& params, float z_pos_,
                      const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_),
      m_surface(surface),
      m_quad(quad),
      m_params(params)
  {
  }

  DrawingParameters& get_params() { return m_params; }

  void render(unsigned int mask) 
  {
    OpenGLState state;
    state.enable(GL_BLEND);
    state.set_blend_func(m_params.blendfunc_src, m_params.blendfunc_dst);
    state.bind_texture(m_surface->get_texture());
    state.activate();

    glPushMatrix();
    glMultMatrixf(modelview.matrix);

    glBegin(GL_QUADS);
    {
      glTexCoord2f(m_surface->get_uv().left, m_surface->get_uv().top);
      glVertex2f(pos.x + m_quad.p1.x, pos.y + m_quad.p1.y);
    
      glTexCoord2f(m_surface->get_uv().right, m_surface->get_uv().top);
      glVertex2f(pos.x + m_quad.p2.x, pos.y + m_quad.p2.y);

      glTexCoord2f(m_surface->get_uv().right, m_surface->get_uv().bottom);
      glVertex2f(pos.x + m_quad.p3.x, pos.y + m_quad.p3.y);

      glTexCoord2f(m_surface->get_uv().left, m_surface->get_uv().bottom);
      glVertex2f(pos.x + m_quad.p4.x, pos.y + m_quad.p4.y);
    }
    glEnd();

    glPopMatrix();    
  }

  void set_quad(const Quad& quad) { m_quad = quad; }
};

#endif

/* EOF */
