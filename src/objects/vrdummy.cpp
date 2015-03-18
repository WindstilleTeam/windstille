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

#include "math/random.hpp"
#include "objects/vrdummy.hpp"

VRDummy::VRDummy(const FileReader& props) :
  sprite(),
  highlight(),
  rotation(),
  jump_time()
{
  props.get("name", name);
  props.get("pos",  pos);

  sprite = Sprite3D(Pathname("models/characters/vrdummy/vrdummy.wsprite"));
  rotation = 0;

  highlight = Sprite(Pathname("images/hedgehog_highlight.sprite"));
  highlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);

  jump_time = 0;
}

VRDummy::~VRDummy()
{

}

void
VRDummy::draw(SceneContext& sc)
{
  sc.highlight().push_modelview();
  sc.highlight().translate(pos.x, pos.y);
  sc.highlight().rotate(rotation, 0.0f, 1.0f, 0.0f);
  sprite.draw(sc.highlight(), Vector2f(0, 0), 1200.0f);
  sc.highlight().pop_modelview();

  sc.highlight().draw(highlight, pos, 1500.0f);
}

void
VRDummy::update(float delta)
{
  sprite.update(delta);
  rotation += delta * 90.0f;
  jump_time += delta;

  if (jump_time >= 1.0f && velocity == Vector2f(0, 0))
  {
    velocity = Vector2f(rnd.frand(-300.0f, 300.0f),
                        rnd.frand(-300.0f, 300.0f));
  }

  if (jump_time >= 1.5f)
  {
    jump_time = 0.0f;

    velocity = Vector2f(0, 0);
  }

  pos += velocity * delta;
}

/* EOF */
