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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_QUAD_DRAWING_REQUEST_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_QUAD_DRAWING_REQUEST_HPP

#include "math/vector2f.hpp"
#include "math/quad.hpp"
#include "display/drawing_request.hpp"

class SurfaceQuadDrawingRequest : public DrawingRequest
{
private:
  Surface surface;
  Quad quad;
  DrawingParameters params;

public:
  SurfaceQuadDrawingRequest(Surface surface_, const Vector2f& pos_, const Quad& quad_, 
                            const DrawingParameters& params_, float z_pos_,
                            const Matrix& modelview_)
    : DrawingRequest(pos_, z_pos_, modelview_),
      surface(surface_),
      quad(quad_),
      params(params_)
  {
  }

  void draw(const Texture& /*tmp_texture*/) 
  {
    OpenGLState state;
    state.enable(GL_BLEND);
    state.set_blend_func(params.blendfunc_src, params.blendfunc_dst);
    state.bind_texture(surface.get_texture());
    state.activate();

    glPushMatrix();
    glMultMatrixf(modelview.matrix);

    glBegin(GL_QUADS);
    {
      glTexCoord2f(surface.get_uv().left, surface.get_uv().top);
      glVertex2f(pos.x + quad.p1.x, pos.y + quad.p1.y);
    
      glTexCoord2f(surface.get_uv().right, surface.get_uv().top);
      glVertex2f(pos.x + quad.p2.x, pos.y + quad.p2.y);

      glTexCoord2f(surface.get_uv().right, surface.get_uv().bottom);
      glVertex2f(pos.x + quad.p3.x, pos.y + quad.p3.y);

      glTexCoord2f(surface.get_uv().left, surface.get_uv().bottom);
      glVertex2f(pos.x + quad.p4.x, pos.y + quad.p4.y);
    }
    glEnd();

    glPopMatrix();    
  }
};

#endif

/* EOF */
