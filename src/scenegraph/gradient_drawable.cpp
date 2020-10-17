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

#include "display/graphics_context.hpp"

GradientDrawable::GradientDrawable(std::vector<float> colors)
  : Drawable(glm::vec2(0, 0), -1000),
    m_array(new VertexArrayDrawable(glm::vec2(0, 0), -1000, glm::mat4(1.0))),
    m_colors(std::move(colors))
{
}

void
GradientDrawable::render(GraphicsContext& gc, unsigned int mask)
{
  m_array->clear();
  if (!m_colors.empty())
  {
    geom::frect rect(0.0f, 0.0f,
                     static_cast<float>(gc.size().width()),
                     static_cast<float>(gc.size().height()));

    m_array->set_mode(GL_TRIANGLE_STRIP);
    m_array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i = 0; i < int(m_colors.size()); i += (3 + 4 + 4 + 2))
    {
      const float& start    = m_colors[i + 0];
      const float& midpoint = m_colors[i + 1];
      const float& end      = m_colors[i + 2];
      const Color color1(m_colors[i + 3], m_colors[i + 4], m_colors[i + 5], m_colors[i + 6]);
      const Color color2(m_colors[i + 7], m_colors[i + 8], m_colors[i + 9], m_colors[i + 10]);
      const Color midcolor((color1.r + color2.r)/2,
                           (color1.g + color2.g)/2,
                           (color1.b + color2.b)/2,
                           (color1.a + color2.a)/2);

      // v2
      m_array->color(color1);
      m_array->vertex(rect.right(), rect.top() + start * rect.height());
      // v1
      m_array->color(color1);
      m_array->vertex(rect.left(), rect.top() + start * rect.height());

      // v4
      m_array->color(midcolor);
      m_array->vertex(rect.right(), rect.top() + midpoint * rect.height());
      // v3
      m_array->color(midcolor);
      m_array->vertex(rect.left(), rect.top() + midpoint * rect.height());

      // v6
      m_array->color(color2);
      m_array->vertex(rect.right(), rect.top() + end * rect.height());
      // v5
      m_array->color(color2);
      m_array->vertex(rect.left(), rect.top() + end * rect.height());
    }
  }

  gc.push_matrix();
  gc.set_modelview(glm::mat4(1.0f));
  m_array->render(gc, mask);
  gc.pop_matrix();
}

/* EOF */
