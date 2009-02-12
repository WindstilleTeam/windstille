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

#include "spider_mine.hpp"
#include "player.hpp"
#include "sprite2d/manager.hpp"

SpiderMine::SpiderMine(FileReader& props)
  : explode("images/explosion.sprite"),
    explode_light("images/explolight.sprite")
{
  props.get("name", name);
  props.get("pos",  pos);
  props.print_unused_warnings("spidermine");
  
  sprite = Sprite("images/spider_mine.sprite");
  //sprite.set_scale(.5, .5);
  initial_position = pos;
  walk_speed = 160;
  exploded = false;
  state = WAIT;
  jump_time = 0.0;
}

SpiderMine::~SpiderMine()
{
}

void
SpiderMine::update(float delta)
{   
  if (state == EXPLODE) {
    explode.update(delta);
    if (explode.is_finished())
      remove();
  } else {
    search_for_player(delta);
    
    if (on_ground()) {
      if (velocity.y > 0) {
        velocity.y = 0;
        pos.y = int(pos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE - 1;
      }

    } else {
      velocity.y += GRAVITY * delta;
    }
  
    pos += velocity * delta;
  }
  
  sprite.update(delta);
}

void
SpiderMine::draw (SceneContext& sc)
{
  if (state == EXPLODE) {
    sc.color().draw(explode, pos);

    sc.light().draw(explode_light, pos, 0);

    explode_light.set_alpha(0.5);
    explode_light.set_scale(0.5f);

    sc.highlight().draw(explode_light, pos, 0);

    explode_light.set_alpha(1.0);
    explode_light.set_scale(1.0);
  }
  else {
    sc.color().draw(sprite, pos, 10.0f);
  }
}

void
SpiderMine::search_for_player(float delta)
{
  Player* player = Player::current();

  jump_time += delta;

  // !line72 - I should be using the real collision stuff for this
  Vector player_pos = player->get_pos();
  
  // check for explosion
  if (state == JUMP && jump_time > 0.3) {
    state = EXPLODE;
    velocity.x = 0;
    velocity.y = 0;
    // if close enough to the player hit them
    if ((fabs(player_pos.x - pos.x) < 15) &&
      ((pos.y - 32 - 15 < player_pos.y) &&
      (pos.y > player_pos.y - 135 + 15))) {
        player->hit(1);
    }
  }
  // check to see if we should jump
  else if (state != JUMP && (fabs(player_pos.x - pos.x) <= 45)
            && ((pos.y - 32 < player_pos.y) && (pos.y > player_pos.y - 135))
            && on_ground()) {
    state = JUMP;
    velocity.y = -400;
    jump_time = 0.0;
  }
  // check to see if we should attack the player
  else if (state != JUMP
           && fabs(player_pos.x - pos.x) < 200
           && fabs(player_pos.x - pos.x) > 45) {
    state = ATTACK;
    if (on_ground()) {
      if (player_pos.x < get_pos().x)
        velocity.x = -walk_speed;
      else
        velocity.x = walk_speed;
    }
  }
  // check to see if we should go back to starting position (ignores y)
  else if (state != JUMP &&
       fabs(player_pos.x - pos.x) >= 200
       && fabs(pos.x - initial_position.x) > 15) {
    state = RETURN;
    if (on_ground()) {
      if (initial_position.x < pos.x)
        velocity.x = -walk_speed;
      else
        velocity.x = walk_speed;
    }
  // else wait
  } else if (state != JUMP) {
    state = WAIT;
    velocity.x = 0;
  }
}

void
SpiderMine::die()
{
  state = EXPLODE;
}


/* EOF */
