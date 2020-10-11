/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SURFACE_DRAWABLE_HPP

#include "scenegraph/surface_drawable.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "display/surface_drawing_parameters.hpp"

class SurfaceDrawable : public Drawable
{
private:
  SurfacePtr surface;
  SurfaceDrawingParameters params;

public:
  SurfaceDrawable(SurfacePtr surface_, const SurfaceDrawingParameters& params_,
                  float z_pos_,
                  const Matrix& modelview_)
    : Drawable(Vector2f(0.0f, 0.0f), z_pos_, modelview_),
      surface(surface_),
      params(params_)
  {}

  ~SurfaceDrawable() override
  {}

  SurfacePtr get_surface() const { return surface; }
  SurfaceDrawingParameters& get_params() { return params; }

  void render(unsigned int mask) override
  {
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(modelview));

    surface->draw(params);

    glPopMatrix();
  }
};

#endif

/* EOF */
