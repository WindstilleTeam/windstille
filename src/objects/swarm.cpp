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

#include "globals.hpp"
#include "player.hpp"
#include "sector.hpp"
#include "hedgehog.hpp"
#include "game_session.hpp"
#include "view.hpp"
#include "display/vertex_array_drawing_request.hpp"
#include "random.hpp"
#include "swarm.hpp"

Swarm::Swarm(FileReader& props)
{
  int count = 100;
  turn_speed = 7.0f;

  props.get("name",  name);
  props.get("pos",   pos);
  props.get("count", count);
  props.get("turn-speed", turn_speed);
  props.print_unused_warnings("swarm");

  agents.resize(count);

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
    {
      i->pos.x = pos.x + rnd.drand(-100, 100);
      i->pos.y = pos.y + rnd.drand(-100, 100);

      i->angle = rnd.drand(-M_PI, M_PI);
      i->speed = rnd.drand(50.0f, 200.0f);

      i->max_speed = rnd.drand(100.0f, 200.0f);

      i->turn_speed = i->max_speed/30.0f;

      i->last_pos = i->pos;
    }
}

void
Swarm::draw(SceneContext& sc)
{
  VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(Vector(0, 0), 
                                                                   1000.0f, sc.highlight().get_modelview());

  array->set_mode(GL_QUADS);
  array->set_blend_func(GL_ONE, GL_ZERO);

  Color color(1.0f, 1.0f, 1.0f);
  Color bottom_color(0.0f, 0.0f, 0.0f);

  for(Agents::const_iterator i = agents.begin(); i != agents.end(); ++i)
    {
      array->color(color);
      array->vertex(i->pos.x - 1, i->pos.y - 1);

      array->color(color);
      array->vertex(i->pos.x + 2, i->pos.y - 1);

      array->color(bottom_color);
      array->vertex(i->pos.x + 2, i->pos.y + 2);

      array->color(bottom_color);
      array->vertex(i->pos.x - 1, i->pos.y + 2);
    }

  sc.highlight().draw(array);
}

static float normalize(float angle)
{
  // brings angle into [0,2*M_PI[ range
  float ret = fmod(fmod(angle, 2*M_PI) + 2*M_PI, 2*M_PI);
  assert(ret >= 0 && ret < 2*M_PI);
  return ret;
}

void
Swarm::update(float delta)
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  
  target = GameSession::current()->get_view()->screen_to_world(Vector(x, y));

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
    {
      i->last_pos = i->pos;

      float dx = target.x - i->pos.x;
      float dy = target.y - i->pos.y;

      float target_angle   = atan2f(dy, dx);
      float relative_angle = normalize(target_angle - i->angle);
      
      if (sqrt(dx*dx + (dy*dy)*2.0f) > 50.0f) // swarm range
        {
          if (fabs(relative_angle) < 0.3f)
            {
              //i->angle += rnd.drand(-1.0f, 1.0f) * delta;
              if (i->speed < i->max_speed) 
                i->speed += 100.0f * delta;
            }
          else
            {
              if (relative_angle <= M_PI)
                i->angle += i->turn_speed * delta;
              else
                i->angle -= i->turn_speed * delta;
            }
        }
      else
        {
          i->angle += rnd.drand(-15.0f, 15.0f) * delta;
          //i->speed += 150.0f - fabs(i->angle);
          i->speed = rnd.drand(50.0f, 100.0f);
        }

      i->pos.x += i->speed * cos(i->angle) * delta;
      i->pos.y += i->speed * sin(i->angle) * delta;
    }
}

/* EOF */
