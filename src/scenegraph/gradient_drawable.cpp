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

#include "scenegraph/gradient_drawable.hpp"

#include <GL/glew.h>

#include "display/display.hpp"

GradientDrawable::GradientDrawable(const std::vector<float>& colors)
  : Drawable(Vector2f(0, 0), -1000),
    array(new VertexArrayDrawable(Vector2f(0, 0), -1000, glm::mat4(1.0)))
{
  if (!colors.empty())
  {
    Rectf rect(0.0f, 0.0f,
               static_cast<float>(Display::get_width()),
               static_cast<float>(Display::get_height()));

    array->set_mode(GL_QUAD_STRIP);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i = 0; i < int(colors.size()); i += (3 + 4 + 4 + 2))
    {
      const float& start    = colors[i + 0];
      const float& midpoint = colors[i + 1];
      const float& end      = colors[i + 2];
      const Color color1(colors[i + 3], colors[i + 4], colors[i + 5], colors[i + 6]);
      const Color color2(colors[i + 7], colors[i + 8], colors[i + 9], colors[i + 10]);
      const Color midcolor((color1.r + color2.r)/2,
                           (color1.g + color2.g)/2,
                           (color1.b + color2.b)/2,
                           (color1.a + color2.a)/2);

      array->color(color1);
      array->vertex(rect.left, rect.top + start * rect.get_height());

      array->color(color1);
      array->vertex(rect.right, rect.top + start * rect.get_height());

      array->color(midcolor);
      array->vertex(rect.left, rect.top + midpoint * rect.get_height());

      array->color(midcolor);
      array->vertex(rect.right, rect.top + midpoint * rect.get_height());

      array->color(color2);
      array->vertex(rect.left, rect.top + end * rect.get_height());

      array->color(color2);
      array->vertex(rect.right, rect.top + end * rect.get_height());
    }
  }
}

void
GradientDrawable::render(unsigned int mask)
{
  glPushMatrix();
  glLoadIdentity();
  array->render(mask);
  glPopMatrix();
}

/* EOF */
