/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/drawing_context.hpp"
#include "util/file_reader.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "particles/particle_system.hpp"
#include "particles/spark_drawer.hpp"

SparkDrawer::SparkDrawer(FileReader& props)
  : color(1.0f, 1.0f, 1.0f),
    width(1.0f),
    buffer()
{
  props.get("color", color);
  props.get("width", width);

  buffer.reset(new VertexArrayDrawable(Vector2f(), 0.0f, Matrix::identity())); 
}

void
SparkDrawer::draw(const ParticleSystem& psys) const
{
  buffer->clear();
  buffer->set_pos(Vector2f(psys.get_x_pos(), psys.get_y_pos()));

  if (width == 1.0f)
    {
      buffer->set_mode(GL_LINES);
      buffer->set_blend_func(GL_SRC_ALPHA, GL_ONE);
      for(ParticleSystem::const_iterator i = psys.begin(); i != psys.end(); ++i)
        {
          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(i->x + i->v_x/10.0f, i->y + i->v_y/10.0f); 

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(i->x, i->y);
        }
    }
  else
    {
      buffer->set_mode(GL_QUADS);
      buffer->set_blend_func(GL_SRC_ALPHA, GL_ONE);
      for(ParticleSystem::const_iterator i = psys.begin(); i != psys.end(); ++i)
        {
          const float len = sqrtf(i->v_x * i->v_x  +  i->v_y * i->v_y);

          const float o_x = i->v_y/len * width;
          const float o_y = i->v_x/len * width;

          const float x1 = i->x;
          const float y1 = i->y;
          const float x2 = i->x + i->v_x/10.0f;
          const float y2 = i->y + i->v_y/10.0f;

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(x1 + o_x, y1 - o_y);

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(x1 - o_x, y1 + o_y);

          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(x2 - o_x, y2 + o_y); 

          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(x2 + o_x, y2 - o_y); 
        }
    }

  buffer->draw(Texture());
}

/* EOF */
