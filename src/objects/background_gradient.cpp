/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "objects/background_gradient.hpp"

#include "engine/sector.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "scenegraph/gradient_drawable.hpp"

BackgroundGradient::BackgroundGradient(const FileReader& props)
  : drawable()
{
  std::vector<float> colors;

  //props.get("z-pos",  z_pos);
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

  drawable.reset(new GradientDrawable(colors));
  Sector::current()->get_scene_graph().add_drawable(drawable);
}

BackgroundGradient::~BackgroundGradient()
{  
}

/* EOF */
