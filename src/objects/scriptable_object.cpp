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
#include "app/console.hpp"
#include "engine/script_manager.hpp"
#include "engine/sector.hpp"
#include "app/globals.hpp"
#include "objects/scriptable_object.hpp"

namespace windstille {

ScriptableObject::ScriptableObject(ReaderMapping const& props) :
  sprite(),
  light(),
  highlight(),
  z_pos(50),
  script_file(),
  target_x(),
  target_y(),
  target_speed(0),
  acceleration(0),
  flash_speed(0),
  flash_delta()
{
  std::string spritename;
  std::string highlightname;
  std::string lightname;

  props.read("name", name);
  props.read("sprite", spritename);
  props.read("highlight", highlightname);
  props.read("light",   lightname);
  props.read("pos", pos);
  props.read("script", script_file);
  props.read("use-verb", use_verb);
  props.read("active", active);
  props.read("flash-speed", flash_speed); // FIXME: bad name, should be something more generic
  props.read("z-pos", z_pos);

  if (use_verb != "")
    set_useable(true);

  if(spritename != "")
    sprite = g_app.sprite().create(Pathname(spritename));

  if (highlightname != "")
  {
    highlight = g_app.sprite().create(Pathname(highlightname));
    highlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  }

  if (lightname != "")
  {
    light = g_app.sprite().create(Pathname(lightname));
    light.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  }

  flash_delta = game_time;
  target_x = pos.x;
  target_y = pos.y;
}

ScriptableObject::~ScriptableObject()
{
}

void
ScriptableObject::draw(wstdisplay::SceneContext& sc)
{
  glm::vec2 tmp_pos = pos;

  if (parent)
    tmp_pos += parent->get_pos();

  if (flash_speed != 0)
    flash();

  if (sprite)
    sprite.draw(sc.color(), tmp_pos, z_pos);

  if (highlight)
    highlight.draw(sc.highlight(), tmp_pos, z_pos);

  if (light)
  {
    light.draw(sc.light(), tmp_pos, z_pos);
  }
}

void
ScriptableObject::update(float delta)
{
  if (sprite)
    sprite.update(delta);

  if (highlight)
    highlight.update(delta);

  if (light)
    light.update(delta);

  if (target_speed > 0)
    move(delta);
}

void
ScriptableObject::use()
{
  if (!script_file.empty())
  {
    try
    {
      Pathname path = Sector::current()->get_directory();
      path.append_path(name).append_text(".nut");
      ScriptManager::current()->run_script_file(path);
    }
    catch (std::exception& e)
    {
      ConsoleLog << e.what() << std::endl;
    }
  }
}

void
ScriptableObject::move_to(float x, float y, float arg_target_speed, float arg_acceleration)
{
  target_x = x;
  target_y = y;
  target_speed = arg_target_speed;
  acceleration = arg_acceleration;
}

void
ScriptableObject::start_flash(float speed)
{
  active = true;
  flash_speed = speed;
  flash_delta = game_time;
}

void
ScriptableObject::move(float delta)
{
  float dx = target_x - pos.x;
  float dy = target_y - pos.y;

  // FIXME: looks fishy
  if (dx == 0.0f && dy == 0.0f) {
    target_speed = 0;
    return;
  }

  //Work out proportion of x/y in direction vector
  float propx = 0;
  float propy = 0;

  float adx = fabsf(dx);
  float ady = fabsf(dy);

  //we can't divide by 0 because we return above if !dx and !dy
  if (dx < 0)
    propx = -(adx / (adx + ady));
  else
    propx = adx / (adx + ady);

  if (dy < 0)
    propy = -(ady / (adx + ady));
  else
    propy = ady / (adx + ady);

  //accelerate
  float dvx = acceleration * delta * propx;
  float dvy = acceleration * delta * propy;
  velocity.x += dvx;
  velocity.y += dvy;

  //don't accelerate past max speed
  float max_vx = fabsf(target_speed * propx);
  float max_vy = fabsf(target_speed * propy);

  if (velocity.x > max_vx)
    velocity.x = max_vx;
  else if (velocity.x < -max_vx)
    velocity.x = -max_vx;

  if (velocity.y > max_vy)
    velocity.y = max_vy;
  else if (velocity.y < -max_vy)
    velocity.y = -max_vy;

  //don't move past the target
  float distx = velocity.x * delta;
  float disty = velocity.y * delta;

  if (adx - fabsf(distx) < 0)
    distx = dx;
  if (ady - fabsf(disty) < 0)
    disty = dy;

  pos.x += distx;
  pos.y += disty;
}

void
ScriptableObject::flash()
{
  float time = game_time - flash_delta;
  if(static_cast<int>(time/flash_speed) % 2 == 0) {
    float alpha = fmodf(time, flash_speed) / flash_speed;
    // fade on
    highlight.set_alpha(alpha);
    light.set_alpha(alpha);
  } else {
    float alpha = 1.0f - (fmodf(time, flash_speed) / flash_speed);
    // fade off
    highlight.set_alpha(alpha);
    light.set_alpha(alpha);
  }
}

} // namespace windstille

/* EOF */
