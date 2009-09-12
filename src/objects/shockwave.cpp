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

#include "objects/shockwave.hpp"

#include "display/opengl_state.hpp"
#include "display/shader_object.hpp"
#include "scenegraph/shockwave_drawable.hpp"

Shockwave::Shockwave(const FileReader& props)
  : pos(),
    noise(Pathname("images/noise3.png")),
    shader_program(),
    radius()
{
  props.get("pos", pos);
  
  radius = 100.0f;

  noise.set_wrap(GL_REPEAT);
  noise.set_filter(GL_LINEAR);

  shader_program.attach(ShaderObject(GL_FRAGMENT_SHADER_ARB, "data/shader/shockwave2.frag"));
  shader_program.link();
}

Shockwave::~Shockwave()
{
}

void
Shockwave::draw (SceneContext& sc)
{
  sc.highlight().draw(new ShockwaveDrawable(pos,
                                                   noise,
                                                   shader_program,
                                                   radius,
                                                   sc.color().get_modelview()));
}

void
Shockwave::update (float delta)
{
  radius += 150.0f * delta;
  if (radius > 300.0f)
    radius = 0;
}

/* EOF */
