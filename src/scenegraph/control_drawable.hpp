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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_CONTROL_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_CONTROL_DRAWABLE_HPP

class ControlDrawable : public Drawable
{
private:
  SurfacePtr surface;
  float angle;

public:
  ControlDrawable(SurfacePtr surface_, const Vector2f& pos_, float angle_, float z_pos_,
                  const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_), 
      surface(surface_), 
      angle(angle_)
  {}

  virtual ~ControlDrawable() {}

  void render(unsigned int mask)
  {
    glPushMatrix();

    // FIXME: This looks badly broken, should modelview.multiply() be enough?
    glTranslatef(modelview.matrix[12],
                 modelview.matrix[13],
                 modelview.matrix[14]);

    Vector3 p = modelview.multiply(Vector3(pos.x, pos.y, 0.0f));

    surface->draw(SurfaceDrawingParameters()
                 .set_angle(angle)
                 .set_pos(Vector2f(p.x - surface->get_width()/2,
                                   p.y - surface->get_height()/2)));

    glPopMatrix();
  }
};

#endif

/* EOF */
