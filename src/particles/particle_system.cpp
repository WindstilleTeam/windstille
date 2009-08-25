/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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
#include "display/scene_context.hpp"
#include "util/file_reader.hpp"
#include "particles/spark_drawer.hpp"
#include "particles/deform_drawer.hpp"
#include "particles/surface_drawer.hpp"
#include "particles/randomizer.hpp"

ParticleSystem::ParticleSystem(FileReader& props)
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
  props.get("bunching", p_bunching);
  set_bunching(p_bunching);

  //FIXME: commented since it causes trouble for the editor: props.get("name", name);
    
  float p_lifetime;
  if (props.get("lifetime", p_lifetime))
    set_lifetime(p_lifetime);

  props.get("z-pos", z_pos);

  Vector2f p_gravity;
  if (props.get("gravity", p_gravity))
    set_gravity(p_gravity.x, p_gravity.y);

  float p_cycles;
  if (props.get("cycles",  p_cycles))
    set_cycles(p_cycles);

  Vector2f spawn_pos;
  if (props.get("spawn-point", spawn_pos))
    set_spawn_point(spawn_pos.x, spawn_pos.y);

  Vector2f p_pos;
  if (props.get("pos", p_pos))
    set_pos(p_pos.x, p_pos.y);
  
  Vector2f p_cone;
  if (props.get("cone", p_cone))
    set_cone(p_cone.x, p_cone.y);

  Vector2f p_size;
  if (props.get("size", p_size))
    set_size(p_size.x, p_size.y);

  props.get("color-begin", color_start);
  props.get("color-end",   color_stop);
  
  Vector2f p_speed;
  if (props.get("velocity", p_speed))
    set_velocity(p_speed.x, p_speed.y);

  {
    std::string layer_str;
    if (props.get("layer", layer_str))
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
    FileReader drawer_reader;
    if (props.get("drawer", drawer_reader))
      {
        std::vector<FileReader> sections = drawer_reader.get_sections();

        if (sections.size() > 1)
          std::cout << "ParticleSystem: Only one drawer allowed" << std::endl;
        
        if (sections.size() == 0)
          std::cout << "ParticleSystem: You must specify a drawer" << std::endl;

        if (sections.size() >= 1)
          {
            FileReader& reader  = sections.front();
            if (reader.get_name() == "surface-drawer") 
              {
                set_drawer(new SurfaceDrawer(reader));
              } 
            else if (reader.get_name() == "spark-drawer") 
              {
                set_drawer(new SparkDrawer(reader));
              } 
            else if (reader.get_name() == "deform-drawer")
              {
                set_drawer(new DeformDrawer(reader));
              }
            else 
              {
                std::cout << "Unknown drawer: " << reader.get_name() << std::endl;
              }
          }
      }
  }

  {
    FileReader distribution_reader;
    if (props.get("distribution", distribution_reader))
      {
        std::vector<FileReader> sections = distribution_reader.get_sections();

        if (sections.size() > 1)
          std::cout << "ParticleSystem: Only one distribution allowed" << std::endl;
        
        if (sections.size() == 0)
          std::cout << "ParticleSystem: You must specify a distribution" << std::endl;

        if (sections.size() >= 1)
          {
            FileReader& reader  = sections.front();

            if (reader.get_name() == "point-distribution") {
              set_point_distribution();
            } else if (reader.get_name() == "line-distribution") {
              float x1, y1, x2, y2;
              reader.get("x1", x1);
              reader.get("y1", y1);
              reader.get("x2", x2);
              reader.get("y2", y2);
          
              set_line_distribution(x1, y1, x2, y2);
            } else if (reader.get_name() == "rect-distribution") {
              Rectf rect;
              reader.get("x1", rect.left);
              reader.get("y1", rect.top);
              reader.get("x2", rect.right);
              reader.get("y2", rect.bottom);
          
              set_rect_distribution(rect);

            } else {
              std::cout << "Unknown distribution: " << reader.get_name() << std::endl;
            }
          }
      }
  }
  
  int p_count = 70;
  props.get("count", p_count);
  set_count(p_count);

  //props.get("point-distribution",   ); // void
  //props.get("line-distribution",   ); // 2xvector2
  //props.get("circle", ); // float
  //props.get("rect-distribution", ); // vector2
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
ParticleSystem::draw(SceneContext& sc)
{
  if (drawer.get())
    {
      drawer->draw(sc.get_layer(layer), *this);
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
  return particles.size();
}

void
ParticleSystem::set_count(int num)
{
  int old_size = particles.size();
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
  assert(!"Implement me");
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
ParticleSystem::get_progress(float t)
{
  return std::max(0.0f, std::min(1.0f, t/life_time));
}

/* EOF */
