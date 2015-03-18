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

#include "app/globals.hpp"
#include "objects/bomb.hpp"

Bomb::Bomb(int x, int y) :
  sprite(Pathname("images/bomb.sprite")),
  explo(Pathname("images/explo.sprite")),
  light(Pathname("images/bomblight.sprite")),
  highlight(Pathname("images/bombhighlight.sprite")),
  explolight(Pathname("images/explolight.sprite")),
  pos(static_cast<float>(x), static_cast<float>((y / TILE_SIZE + 1) * TILE_SIZE)),
  count(2.0f),
  state(COUNTDOWN),
  exploded(false)
{
}

Bomb::~Bomb()
{
}

void
Bomb::update(float delta)
{
  if (explo.is_finished())
    remove();

  if (state == EXPLODE)
    explo.update(delta);
  else
    sprite.update(delta);

  count -= delta;

  if (count < 0 && state != EXPLODE)
  {
    state = EXPLODE;
    count = 0;
    if (!exploded)
    {
      exploded = true;
      explode();
    }

  }
}

void
Bomb::draw(SceneContext& sc)
{
  if (state == EXPLODE)
  {
    sc.color().draw(explo, pos);
    sc.light().draw(explolight, pos, 0);

    explolight.set_alpha(0.5);
    explolight.set_scale(0.5);

    sc.highlight().draw(explolight, pos, 0);

    explolight.set_alpha(1.0);
    explolight.set_scale(1.0);
  }
  else
  {
    sc.color().draw(sprite, pos);
    if (sprite.is_finished()) {
      sc.light().draw(light, pos, 0);
      sc.highlight().draw(highlight, pos, 0);
    }
  }
}

void
Bomb::explode()
{
}

/* EOF */
