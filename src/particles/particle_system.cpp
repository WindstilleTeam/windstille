/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "particles/particle_system.hpp"

#include <iostream>

#include "display/scene_context.hpp"
#include "particles/deform_drawer.hpp"
#include "particles/randomizer.hpp"
#include "particles/spark_drawer.hpp"
#include "particles/surface_drawer.hpp"
#include "util/file_reader.hpp"
#include "util/log.hpp"

ParticleSystem::ParticleSystem(ReaderMapping const& props,
                               SurfaceManager& surface_manager)
  : particles(),
    life_time(1.0f),
    randomizer(new PointRandomizer()),
    drawer(),
    spawn_x(0),
    spawn_y(0),
    x_pos(320.0f),
    y_pos(240.0f),
    z_pos(0),
    gravity_x(0.0f),
    gravity_y(-10.0f),
    cone_start(0),
    cone_stop(2.0f * math::pi),
    bunching(1.0f),
    layer(SceneContext::COLORMAP),
    size_start(1.0f),
    size_stop(1.0f),
    speed_start(100.0),
    speed_stop(200.0f),
    color_start(1.0f, 1.0f, 1.0f, 1.0f),
    color_stop(   0,    0,    0,    0)
{
  float p_bunching = 1.0;
  props.read("bunching", p_bunching);
  set_bunching(p_bunching);

  //FIXME: commented since it causes trouble for the editor: props.get("name", name);

  float p_lifetime;
  if (props.read("lifetime", p_lifetime))
    set_lifetime(p_lifetime);

  props.read("z-pos", z_pos);

  glm::vec2 p_gravity;
  if (props.read("gravity", p_gravity))
    set_gravity(p_gravity.x, p_gravity.y);

  float p_cycles;
  if (props.read("cycles",  p_cycles))
    set_cycles(p_cycles);

  glm::vec2 spawn_pos;
  if (props.read("spawn-point", spawn_pos))
    set_spawn_point(spawn_pos.x, spawn_pos.y);

  glm::vec2 p_pos;
  if (props.read("pos", p_pos))
    set_pos(p_pos.x, p_pos.y);

  glm::vec2 p_cone;
  if (props.read("cone", p_cone))
    set_cone(p_cone.x, p_cone.y);

  glm::vec2 p_size;
  if (props.read("size", p_size))
    set_size(p_size.x, p_size.y);

  props.read("color-begin", color_start);
  props.read("color-end",   color_stop);

  glm::vec2 p_speed;
  if (props.read("velocity", p_speed))
    set_velocity(p_speed.x, p_speed.y);

  {
    std::string layer_str;
    if (props.read("layer", layer_str))
    {
      if (layer_str == "highlight")
        layer = SceneContext::HIGHLIGHTMAP;
      else if (layer_str == "light")
        layer = SceneContext::LIGHTMAP;
      else if (layer_str == "color")
        layer = SceneContext::COLORMAP;
      else
        std::cout << "ParticleSystem: Unknown layer type: '" << layer_str << "'" << std::endl;
    }
  }

  {
    ReaderObject drawer_obj;
    if (props.read("drawer", drawer_obj)) {
      if (drawer_obj.get_name() == "surface-drawer") {
        set_drawer(new SurfaceDrawer(drawer_obj.get_mapping(), surface_manager));
      } else if (drawer_obj.get_name() == "spark-drawer") {
        set_drawer(new SparkDrawer(drawer_obj.get_mapping()));
      } else if (drawer_obj.get_name() == "deform-drawer") {
        set_drawer(new DeformDrawer(drawer_obj.get_mapping(), surface_manager));
      } else {
        std::cout << "Unknown drawer: " << drawer_obj.get_name() << std::endl;
      }
    }
  }

  {
    ReaderObject distribution_obj;
    if (props.read("distribution", distribution_obj))
    {
      ReaderMapping const& reader = distribution_obj.get_mapping();

      if (distribution_obj.get_name() == "point-distribution") {
        set_point_distribution();
      } else if (distribution_obj.get_name() == "line-distribution") {
        float x1, y1, x2, y2;
        reader.read("x1", x1);
        reader.read("y1", y1);
        reader.read("x2", x2);
        reader.read("y2", y2);

        set_line_distribution(x1, y1, x2, y2);
      } else if (distribution_obj.get_name() == "rect-distribution") {
        float x1 = 0.0f;
        float y1 = 0.0f;
        float x2 = 0.0f;
        float y2 = 0.0f;
        reader.read("x1", x1);
        reader.read("y1", y1);
        reader.read("x2", x2);
        reader.read("y2", y2);

        set_rect_distribution(Rectf(x1, y1, x2, y2));
      } else {
        std::cout << "Unknown distribution: " << distribution_obj.get_name() << std::endl;
      }
    }
  }

  int p_count = 70;
  props.read("count", p_count);
  set_count(p_count);

  //props.read("point-distribution",   ); // void
  //props.read("line-distribution",   ); // 2xvector2
  //props.read("circle", ); // float
  //props.read("rect-distribution", ); // vector2
}

ParticleSystem::ParticleSystem()
  : particles(),
    life_time(1.0f),
    randomizer(new PointRandomizer()),
    drawer(),
    spawn_x(0),
    spawn_y(0),
    x_pos(320.0f),
    y_pos(240.0f),
    z_pos(0),
    gravity_x(0.0f),
    gravity_y(-10.0f),
    cone_start(0),
    cone_stop(2.0f * math::pi),
    bunching(1.0f),
    layer(SceneContext::COLORMAP),
    size_start(1.0f),
    size_stop(1.0f),
    speed_start(100.0),
    speed_stop(200.0f),
    color_start(1.0f, 1.0f, 1.0f, 1.0f),
    color_stop(   0,    0,    0,    0)
{
  set_count(70);
}

ParticleSystem::~ParticleSystem()
{
}

void
ParticleSystem::set_drawer(Drawer* drawer_)
{
  drawer.reset(drawer_);
}

void
ParticleSystem::draw() const
{
  if (drawer)
  {
    drawer->draw(*this);
  }
  else
  {
    std::cout << "ParticleSystem: No drawer set" << std::endl;
  }
}

void
ParticleSystem::spawn(Particle& particle)
{
  randomizer->set_pos(particle);

  particle.x   += x_pos + spawn_x;
  particle.y   += y_pos + spawn_y;
  // FIXME: parent handling disabled due to work on the editor
  // particle.x   += (parent ? parent->get_pos().x : 0) + x_pos + spawn_x;
  // particle.y   += (parent ? parent->get_pos().y : 0) + y_pos + spawn_y;

  float direction = rnd.frand(cone_start, cone_stop);
  float speed     = rnd.frand(speed_start, speed_stop);
  particle.v_x    = cosf(direction) * speed;
  particle.v_y    = sinf(direction) * speed;

  particle.angle = rnd.frand(360.0f);

  particle.t   = fmodf(particle.t, life_time);
}

void
ParticleSystem::update(float delta)
{
  for(Particles::iterator i = particles.begin(); i != particles.end(); ++i)
  {
    if (i->t > life_time)
    {
      spawn(*i);
    }
    else
    {
      i->t += delta;

      i->x += i->v_x * delta;
      i->y += i->v_y * delta;

      i->v_x += gravity_x;
      i->v_y += gravity_y;
    }
  }
}

int
ParticleSystem::get_count() const
{
  return static_cast<int>(particles.size());
}

void
ParticleSystem::set_count(int num)
{
  int old_size = static_cast<int>(particles.size());
  if (old_size != num)
  {
    particles.resize(num);

    for(Particles::size_type i = old_size; i < particles.size(); ++i)
    {
      spawn(particles[i]);
      particles[i].t = life_time * bunching * static_cast<float>(i) / static_cast<float>(particles.size());
    }
  }
}

void
ParticleSystem::set_bunching(float factor)
{
  bunching = std::max(0.0f, std::min(factor, 1.0f));
}

void
ParticleSystem::set_cycles(float /*num*/)
{
  log_debug("Implement me");
}

void
ParticleSystem::set_pos(const glm::vec2& pos)
{
  x_pos = pos.x;
  y_pos = pos.y;
}

void
ParticleSystem::set_pos(float x, float y)
{
  x_pos = x;
  y_pos = y;
}

void
ParticleSystem::set_spawn_point(float x, float y)
{
  spawn_x = x;
  spawn_y = y;
}

void
ParticleSystem::set_point_distribution()
{
  randomizer.reset(new PointRandomizer());
}

void
ParticleSystem::set_line_distribution(float x1, float y1,
                                      float x2, float y2)
{
  randomizer.reset(new LineRandomizer(x1, y1, x2, y2));
}

void
ParticleSystem::set_circle_distribution(float radius)
{
  randomizer.reset(new CircleRandomizer(radius));
}

void
ParticleSystem::set_rect_distribution(const Rectf& rect)
{
  randomizer.reset(new RectRandomizer(rect));
}

void
ParticleSystem::set_cone(float start_angle, float stop_angle)
{
  cone_start = start_angle * math::pi/180.0f;
  cone_stop  = stop_angle  * math::pi/180.0f;
}

void
ParticleSystem::set_gravity(float arg_gravity_x, float arg_gravity_y)
{
  gravity_x = arg_gravity_x;
  gravity_y = arg_gravity_y;
}

void
ParticleSystem::set_lifetime(float time)
{
  life_time = time;
}

void
ParticleSystem::set_size(float from, float to)
{
  size_start = from;
  size_stop  = to;
}

void
ParticleSystem::set_color(const Color& color_start_, const Color& color_end_)
{
  color_start = color_start_;
  color_stop  = color_end_;
}

void
ParticleSystem::set_fade_color(const Color& color)
{
  color_stop = color;
}

void
ParticleSystem::set_velocity(float from, float to)
{
  speed_start = from;
  speed_stop  = to;
}

float
ParticleSystem::get_progress(float t) const
{
  return std::max(0.0f, std::min(1.0f, t/life_time));
}

/* EOF */
