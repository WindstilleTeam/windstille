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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_QUAD_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_QUAD_DRAWABLE_HPP

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <geom/quad.hpp>

#include <wstdisplay/scenegraph/drawable.hpp>

namespace wstdisplay {

class SurfaceQuadDrawable : public Drawable
{
private:
  SurfacePtr m_surface;
  geom::fquad m_quad;
  DrawingParameters m_params;

public:
  SurfaceQuadDrawable(SurfacePtr surface, const geom::fpoint& pos_, const geom::fquad& quad,
                      const DrawingParameters& params, float z_pos_,
                      const glm::mat4& modelview_)
    : Drawable(pos_, z_pos_, modelview_),
      m_surface(surface),
      m_quad(quad),
      m_params(params)
  {
  }

  DrawingParameters& get_params() { return m_params; }

  void render(GraphicsContext& gc, unsigned int mask) override
  {
    VertexArrayDrawable va;

    va.set_blend_func(m_params.blendfunc_src, m_params.blendfunc_dst);
    va.set_texture(m_surface->get_texture());

    gc.push_matrix();
    gc.mult_matrix(modelview);

    va.set_mode(GL_TRIANGLE_FAN);
    {
      va.texcoord(m_surface->get_uv().left(), m_surface->get_uv().top());
      va.vertex(pos.x() + m_quad.p1.x(), pos.y() + m_quad.p1.y());

      va.texcoord(m_surface->get_uv().right(), m_surface->get_uv().top());
      va.vertex(pos.x() + m_quad.p2.x(), pos.y() + m_quad.p2.y());

      va.texcoord(m_surface->get_uv().right(), m_surface->get_uv().bottom());
      va.vertex(pos.x() + m_quad.p3.x(), pos.y() + m_quad.p3.y());

      va.texcoord(m_surface->get_uv().left(), m_surface->get_uv().bottom());
      va.vertex(pos.x() + m_quad.p4.x(), pos.y() + m_quad.p4.y());
    }
    va.render(gc);

    gc.pop_matrix();
  }

  void set_quad(const geom::fquad& quad) { m_quad = quad; }
};

} // namespace wstdisplay

#endif

/* EOF */
