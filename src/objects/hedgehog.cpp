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
#include "player.hpp"
#include "engine/sector.hpp"
#include "hedgehog.hpp"

Hedgehog::Hedgehog(FileReader& props)
  : sprite("images/hedgehog.sprite"),
    die_sprite("images/hedgehog_die1.sprite"),
    light("images/hedgehog_light.sprite"),
    highlight("images/hedgehog_highlight.sprite")
{
  props.get("name", name);
  props.get("pos",  pos);
  props.print_unused_warnings("hedgehog");
  
  direction_left = false;
  state = WALKING;
  light.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  highlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);
}

Hedgehog::~Hedgehog()
{
}

void
Hedgehog::draw(SceneContext& gc)
{
  Sprite* s;

  if (state == DYING)
    s = &die_sprite;
  else
    s = &sprite;

  if (direction_left)
    s->set_vflip(true);
  else
    s->set_vflip(false);

  gc.color().draw(*s, pos, 2);
  //s->draw(int(pos.x), int(pos.y));
  gc.light().draw(light, pos, 0);
  gc.highlight().draw(highlight, pos, 0);
}

void
Hedgehog::update(float delta)
{      
  if (state == DYING)
    {
      if (die_sprite.is_finished())
        remove();
      die_sprite.update(delta);
    }
  else
    {
      sprite.update(delta);
      bool was_on_ground = false;
      
      if (on_ground())
        {
          was_on_ground = true;
          if (velocity.y > 0)
            {
              velocity.y = 0;
              pos.y = int(pos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE - 1;
            }
          if (direction_left)
            velocity.x = -32;
          else
            velocity.x = 32;
        }
      else
        {
          velocity.y += GRAVITY * delta;
        }
      
      Vector old_pos = pos;
      pos += velocity * delta;
      
      if ((was_on_ground && !on_ground()) || in_wall())
        {
          direction_left = !direction_left;
          pos = old_pos;
        }
    }
    
  // Check if the player got hit
  // FIXME: Insert pixel perfect collision detection here
  Vector player_pos = Player::current()->get_pos();
  if (pos.x - 20 < player_pos.x
        && pos.x + 20 > player_pos.x
        && pos.y - 20 < player_pos.y
        && pos.y + 5  > player_pos.y)
    Player::current()->hit(5);
}

void
Hedgehog::die()
{
  state = DYING;
}

/* EOF */
