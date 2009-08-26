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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_DRAWING_REQUEST_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_DRAWING_REQUEST_HPP

#include "display/surface_drawing_request.hpp"
#include "display/surface_drawing_parameters.hpp"

class SurfaceDrawingRequest : public DrawingRequest
{
private:
  Surface surface;
  SurfaceDrawingParameters params;

public:
  SurfaceDrawingRequest(Surface surface_, const SurfaceDrawingParameters& params_,
                        float z_pos_,
                        const Matrix& modelview_)
    : DrawingRequest(pos, z_pos_, modelview_), 
      surface(surface_), 
      params(params_)
  {}

  virtual ~SurfaceDrawingRequest()
  {}

  void draw(const Texture& /*tmp_texture*/) 
  {
    glPushMatrix();
    glMultMatrixf(modelview.matrix);

    surface.draw(params);

    glPopMatrix();
  }
};

#endif

/* EOF */
