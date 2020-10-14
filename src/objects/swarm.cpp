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

#include "objects/swarm.hpp"

#include <numbers>
#include <glm/gtx/fast_trigonometry.hpp>

#include "math/math.hpp"
#include "math/random.hpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "screen/game_session.hpp"
#include "screen/view.hpp"

Swarm::Swarm(ReaderMapping const& props) :
  agents(),
  target(),
  turn_speed()
{
  int count = 100;
  turn_speed = 7.0f;

  props.read("name",  name);
  props.read("pos",   pos);
  props.read("count", count);
  props.read("turn-speed", turn_speed);

  agents.resize(count);

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
  {
    i->pos.x = pos.x + rnd.frand(-100, 100);
    i->pos.y = pos.y + rnd.frand(-100, 100);

    i->angle = rnd.frand(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
    i->speed = rnd.frand(50.0f, 200.0f);

    i->max_speed = rnd.frand(100.0f, 200.0f);

    i->turn_speed = i->max_speed/30.0f;

    i->last_pos = i->pos;
  }
}

void
Swarm::draw(SceneContext& sc)
{
  auto array = std::make_unique<VertexArrayDrawable>(glm::vec2(0, 0),
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

  sc.highlight().draw(std::move(array));
}

void
Swarm::update(float delta)
{
  int x, y;
  SDL_GetMouseState(&x, &y);

  target = GameSession::current()->get_view()->screen_to_world(glm::vec2(static_cast<float>(x), static_cast<float>(y)));

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
  {
    i->last_pos = i->pos;

    float dx = target.x - i->pos.x;
    float dy = target.y - i->pos.y;

    float target_angle   = atan2f(dy, dx);
    float relative_angle = glm::wrapAngle(target_angle - i->angle);

    if (sqrtf(dx*dx + (dy*dy)*2.0f) > 50.0f) // swarm range
    {
      if (fabsf(relative_angle) < 0.3f)
      {
        //i->angle += rnd.frand(-1.0f, 1.0f) * delta;
        if (i->speed < i->max_speed)
          i->speed += 100.0f * delta;
      }
      else
      {
        if (relative_angle <= std::numbers::pi_v<float>)
          i->angle += i->turn_speed * delta;
        else
          i->angle -= i->turn_speed * delta;
      }
    }
    else
    {
      i->angle += rnd.frand(-15.0f, 15.0f) * delta;
      //i->speed += 150.0f - fabs(i->angle);
      i->speed = rnd.frand(50.0f, 100.0f);
    }

    i->pos.x += i->speed * cosf(i->angle) * delta;
    i->pos.y += i->speed * sinf(i->angle) * delta;
  }
}

/* EOF */
