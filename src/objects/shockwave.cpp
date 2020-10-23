/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "app/app.hpp"
#include <wstdisplay/shader_object.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/scenegraph/shockwave_drawable.hpp>
#include "util/pathname.hpp"

Shockwave::Shockwave(ReaderMapping const& props) :
  pos(),
  noise(g_app.texture().get(Pathname("images/noise3.png"))),
  shader_program(ShaderProgram::create()),
  radius()
{
  props.read("pos", pos);

  radius = 100.0f;

  noise->set_wrap(GL_REPEAT);
  noise->set_filter(GL_LINEAR);

  shader_program->attach(ShaderObject::from_file(GL_FRAGMENT_SHADER, "data/shader/shockwave2.frag"));
  shader_program->link();
}

Shockwave::~Shockwave()
{
}

void
Shockwave::draw (SceneContext& sc)
{
  sc.highlight().draw(std::make_unique<ShockwaveDrawable>(pos,
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
