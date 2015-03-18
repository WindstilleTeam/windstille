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

#ifndef HEADER_WINDSTILLE_PARTICLES_PARTICLE_SYSTEM_HPP
#define HEADER_WINDSTILLE_PARTICLES_PARTICLE_SYSTEM_HPP

#include <boost/scoped_ptr.hpp>
#include <vector>

#include "math/rect.hpp"
#include "display/color.hpp"
#include "particles/drawer.hpp"

class FileReader;
class SceneContext;
class Randomizer;
struct Particle;
class ParticleSystem;

struct Particle
{
  /** Position of the particle */
  float x;
  float y;

  /** Velocity of the particle */
  float v_x;
  float v_y;

  /** Rotation angle */
  float angle;

  /** How long the particle already lifed, -1 means the particle
      isn't active and ready to respawn */
  float t;
};

class ParticleSystem
{
public:
  typedef std::vector<Particle> Particles;
  typedef Particles::iterator iterator;
  typedef Particles::const_iterator const_iterator;

private:
  Particles particles;

  float life_time;

  /** Places the particle in its initial position */
  boost::scoped_ptr<Randomizer> randomizer;
  boost::scoped_ptr<Drawer>     drawer;

  float spawn_x;
  float spawn_y;

  float x_pos;
  float y_pos;
  float z_pos;

  float gravity_x;
  float gravity_y;

  float cone_start;
  float cone_stop;

  float bunching;

  /** Type of layer to which this particle system gets drawn */
  unsigned int layer;

  float size_start;
  float size_stop;

  float speed_start;
  float speed_stop;

  Color color_start;
  Color color_stop;

private:
  void spawn(Particle& particle);

public:
  ParticleSystem();
  ParticleSystem(FileReader& props);
  ~ParticleSystem();

  void set_drawer(Drawer*);

  /** Draws the particle system to the screen */
  void draw() const;

  /** Update the particle system \a delta seconds */
  void update(float delta);

  /** Set how many particles will be used */
  void set_count(int num);

  int get_count() const;

  /** \a factor == 1 means a constant stream of particles, a value of
      0 means all particles will be released at once */
  void set_bunching(float factor);

  /** Set how often the particle generator restarts itself, value of 0
      means it will work forever, a value of 1 would cause all
      particles to be emmitted only once */
  void set_cycles(float num);

  /** The position of the particle system in world coordinates */
  void set_pos(float x, float y);

  /** The position of the particle system in world coordinates */
  void set_pos(const Vector2f& pos);

  /** The position from which the particles spawn, x,y are in world
      coordinates, this is relative to the position you can set via
      set_pos() */
  void set_spawn_point(float x, float y);

  /** Causes all particles to spawn from a single point */
  void set_point_distribution();

  /** Causes all particles to spawn from a line */
  void set_line_distribution(float x1, float y1,
                             float x2, float y2);

  /** Causes particles to not be spawned at a single point, but inside
      the given circle */
  void set_circle_distribution(float radius);

  /** Causes particles to not be spawned at a single point, but inside
      the given rectangle */
  void set_rect_distribution(const Rectf& rect);

  /** Limit the direction into which the new particles spawn by the
      given angles, angles are given in degrees */
  void set_cone(float start_angle, float stop_angle);

  /** The gravitiy that pulls the praticles up/down/left/right */
  void set_gravity(float gravity_x, float gravity_y);

  /** Set how long the particles will be alive */
  void set_lifetime(float time);

  /** Set the size of the particles, size will scale \a from to \a to
      along their lifetime */
  void set_size  (float from, float to);

  /** Set the color at which the particles will start */
  void set_color(const Color& start, const Color& end);

  /** Set the color at which the particles will end */
  void set_fade_color(const Color& color);

  /** Set the velocity of the particles, it will be randomly distributed
      from \a from to \a to, direction will be taken from the cone */
  void set_velocity(float from, float to);

  iterator begin() { return particles.begin(); }
  iterator end()   { return particles.end(); }

  const_iterator begin() const { return particles.begin(); }
  const_iterator end()   const { return particles.end(); }

  float get_size_start() const { return size_start; }
  float get_size_stop()  const { return size_stop; }

  const Color& get_color_start() const { return color_start; }
  const Color& get_color_stop()  const { return color_stop;  }

  float get_x_pos() const { return x_pos; }
  float get_y_pos() const { return y_pos; }
  float get_z_pos() const { return z_pos; }

  /** Returns the how much a particle has progressed, this is
      particle.life_time with fade_in/fade_out applied  */
  float get_progress(float t) const;

  unsigned int get_layer() const { return layer; }

private:
  ParticleSystem (const ParticleSystem&);
  ParticleSystem& operator= (const ParticleSystem&);
};

#endif

/* EOF */
