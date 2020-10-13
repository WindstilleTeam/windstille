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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_CONTROL_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_CONTROL_DRAWABLE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ControlDrawable : public Drawable
{
private:
  SurfacePtr surface;
  float angle;

public:
  ControlDrawable(SurfacePtr surface_, const glm::vec2& pos_, float angle_, float z_pos_,
                  const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_),
      surface(surface_),
      angle(angle_)
  {}

  ~ControlDrawable() override {}

  void render(unsigned int mask) override
  {
    glPushMatrix();

    // FIXME: This looks badly broken, should modelview.multiply() be enough?
    glTranslatef(glm::value_ptr(modelview)[12],
                 glm::value_ptr(modelview)[13],
                 glm::value_ptr(modelview)[14]);

    glm::vec3 p = glm::vec3(modelview * glm::vec4(pos.x, pos.y, 0.0f, 1.0f));

    surface->draw(SurfaceDrawingParameters()
                 .set_angle(angle)
                 .set_pos(glm::vec2(p.x - surface->get_width()/2,
                                   p.y - surface->get_height()/2)));

    glPopMatrix();
  }
};

#endif

/* EOF */
