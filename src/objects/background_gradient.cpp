/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "display/vertex_array_drawing_request.hpp"
#include "background_gradient.hpp"

BackgroundGradient::BackgroundGradient(FileReader& props)
{
  z_pos = 0.0;

  props.get("z-pos",  z_pos);
  props.get("colors", colors);
  if (colors.size() % (3 + 4 + 4 + 2) != 0)
    {
      std::cout << "BackgroundGradient: specified color gradient is invalid" << std::endl;
      /** Color gradients are in the format:
          
      (colors start midpoint end R1 G1 B1 A1 R2 G2 B2 A2 I I
              start midpoint end R1 G1 B1 A1 R2 G2 B2 A2 I I
              ...)

              I is ignored

          all specified in float, this is similar to Gimps gradients
          so you can easily copy&paste
       */
      colors.clear();
    }
}

BackgroundGradient::~BackgroundGradient()
{  
}
  
void
BackgroundGradient::update(float delta)
{
}

void
BackgroundGradient::draw(SceneContext& sc)
{
  if (colors.empty())
    return ;

  // Reset modelview so we can draw in screen space
  sc.color().push_modelview();
  sc.color().set_modelview(Matrix::identity());
  
  Color topcolor(0.0f, 0.0f, 0.5f);
  Color bottomcolor(0.5f, 0.5f, 1.0f);

  Rect rect(0, 0, 800, 600);
  VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(Vector(0, 0), z_pos, 
                                                                   sc.color().get_modelview());

  array->set_mode(GL_QUAD_STRIP);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for(int i = 0; i < int(colors.size()); i += (3 + 4 + 4 + 2))
    {
      float& start    = colors[i + 0];
      float& midpoint = colors[i + 1];
      float& end      = colors[i + 2];
      Color color1(colors[i + 3], colors[i + 4], colors[i + 5], colors[i + 6]);
      Color color2(colors[i + 7], colors[i + 8], colors[i + 9], colors[i + 10]);
      Color midcolor((color1.r + color2.r)/2,
                     (color1.g + color2.g)/2,
                     (color1.b + color2.b)/2,
                     (color1.a + color2.a)/2);

      array->color(color1);
      array->vertex(rect.left, rect.top + start*rect.get_height());

      array->color(color1);
      array->vertex(rect.right, rect.top + start*rect.get_height());

      array->color(midcolor);
      array->vertex(rect.left, rect.top + midpoint*rect.get_height());

      array->color(midcolor);
      array->vertex(rect.right, rect.top + midpoint*rect.get_height());

      array->color(color2);
      array->vertex(rect.left, rect.top + end*rect.get_height());

      array->color(color2);
      array->vertex(rect.right, rect.top + end*rect.get_height());  
    }

  sc.color().draw(array);  

  sc.color().pop_modelview();
}

/* EOF */