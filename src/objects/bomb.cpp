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

#include "app/app.hpp"
#include "app/globals.hpp"
#include "objects/bomb.hpp"
#include "util/pathname.hpp"

Bomb::Bomb(int x, int y) :
  sprite(g_app.sprite().create(Pathname("images/bomb.sprite"))),
  explo(g_app.sprite().create(Pathname("images/explo.sprite"))),
  light(g_app.sprite().create(Pathname("images/bomblight.sprite"))),
  highlight(g_app.sprite().create(Pathname("images/bombhighlight.sprite"))),
  explolight(g_app.sprite().create(Pathname("images/explolight.sprite"))),
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
    explo.draw(sc.color(), pos);
    explolight.draw(sc.light(), pos, 0);

    explolight.set_alpha(0.5);
    explolight.set_scale(0.5);

    explolight.draw(sc.highlight(), pos, 0);

    explolight.set_alpha(1.0);
    explolight.set_scale(1.0);
  }
  else
  {
    sprite.draw(sc.color(), pos);
    if (sprite.is_finished()) {
      light.draw(sc.light(), pos, 0);
      highlight.draw(sc.highlight(), pos, 0);
    }
  }
}

void
Bomb::explode()
{
}

/* EOF */
