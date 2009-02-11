/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_PLAYER_HPP
#define HEADER_WINDSTILLE_PLAYER_HPP

#include <memory>
#include "sprite2d/sprite.hpp"
#include "input/controller.hpp"
#include "sprite3d/sprite3d.hpp"
#include "sound/sound_source.hpp"
#include "math/vector.hpp"
#include "globals.hpp"
#include "engine/entity.hpp"
#include "util/ref.hpp"
#include "display/surface.hpp"

class Controller;
class Contact;
class LaserPointer;
class Weapon;

class Player : public Entity
{
private:
  Controller controller;

  Sprite   light;
  Sprite   flashlight;
  Sprite   flashlighthighlight;
  Sprite3D sprite;
  std::auto_ptr<SoundSource> sound_source;

  bool jumping;
  bool bomb_placed;
  float hit_count;
  int energy;

  CollisionObject* c_object;

  LaserPointer* laser_pointer;
  Contact* contact;
  Weapon* weapon;

public:
  enum State
  {
    STAND,
    WALK,
    TURNAROUND,
    DUCKING,
    DUCKED,
    ACCELERATE,
    RUN,
    PULL_GUN,
    SLOWDOWN,
    STAND_TO_LISTEN,
    LISTEN,
    JUMP_BEGIN,
    JUMP_AIR,
    JUMP_LAND,
    JUMP_UP_BEGIN,
    JUMP_UP_AIR,
    JUMP_UP_LAND,
    STAIRS_UP,
    STAIRS_DOWN
  };

private:
  State state;

  // jump state info
  enum Foot {
    LEFT_FOOT, RIGHT_FOOT
  };
  Foot jump_foot;
  
  double reload_time;
  float  z_pos;
  static Player* current_;
public:
  Player ();
  virtual ~Player ();

  static Player* current() { return current_; }

  int get_movement_state() { return state; }

  void draw (SceneContext& gc);
  void update (float delta);

  void start_listening();
  void stop_listening();
  
  int get_energy() const;
  int get_max_energy() const;
  void hit(int points);
  
  void collision(const CollisionData& data);

  Entity* find_useable_entity();

  virtual void set_pos(Vector pos);

private:
  /*
   * Following is a set of functions that manage the players state
   * the set_ function is called to enter a new state, the update function is
   * called each frame when a state is active, the leave function is called
   * right before we enter a new state.
   *
   * TODO: can we generalize this mechanism? Or create a small specification
   * language for this?
   */
  
  void update_walk_stand();
  void set_stand();
  void update_stand();
  void set_walk(Direction direction);
  void update_walk();
  void leave_walk();
  void set_ducking();
  void update_ducking();
  void set_ducked();
  void update_ducked();
  void set_turnaround();
  void update_turnaround();
  void set_stand_to_listen(bool backwards);
  void update_stand_to_listen();
  void set_listen();
  void update_listen();
  void set_run();
  void update_run();
  void leave_run();
  
  void set_jump_begin();
  void update_jump_begin();
  void set_jump_air();
  void update_jump_air();
  void set_jump_land();
  void update_jump_land();
  
  void set_jump_up_begin();
  void update_jump_up_begin();
  void set_jump_up_air();
  void update_jump_up_air();
  void set_jump_up_land();
  void update_jump_up_land();

  void update_pull_gun();
  void update_stairs(float delta);

  /**
   * Sets an action for the sprite. In contrast to sprite->set_action this
   * function will not restart the action if it was already running
   */
  void try_set_action(const std::string& name, float speed = 1.0);

  Direction get_direction() const;
};

#endif

/* EOF */
