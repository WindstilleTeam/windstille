/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_PLAYER_HPP
#define HEADER_WINDSTILLE_OBJECTS_PLAYER_HPP

#include <wstinput/controller.hpp>

#include "app/globals.hpp"
#include <wstdisplay/surface.hpp>
#include "engine/entity.hpp"
#include <glm/glm.hpp>
#include "sprite2d/sprite.hpp"
#include "sprite3d/sprite3d.hpp"
#include "util/currenton.hpp"

namespace windstille {

class Contact;
class LaserPointer;
class CollisionObject;
class CollisionData;
class Weapon;
class Sprite3DDrawable;

using Controller = wstinput::Controller;

class Player : public Entity,
               public Currenton<Player>
{
private:
  std::shared_ptr<Sprite3DDrawable> m_drawable;

  bool jumping;
  bool bomb_placed;
  float hit_count;
  int energy;

  CollisionObject* c_object;

  LaserPointer* laser_pointer;
  Contact* contact;
  std::unique_ptr<Weapon> weapon;

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

  enum Direction { WEST, EAST };

  double reload_time;
  float  z_pos;

public:
  Player ();
  ~Player () override;

  int get_movement_state() { return state; }

  void draw(wstdisplay::SceneContext& gc) override;
  void update(float delta) override;
  void update(wstinput::Controller const& controller, float delta);

  void start_listening();
  void stop_listening();

  int get_energy() const;
  int get_max_energy() const;
  void hit(int points);

  void collision(CollisionData const& data);

  Entity* find_useable_entity();

  void set_pos(glm::vec2 pos) override;

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
  void update_ducked(wstinput::Controller const& controller);
  void update_ducking(wstinput::Controller const& controller);
  void update_listen(wstinput::Controller const& controller);
  void update_run(wstinput::Controller const& controller);
  void update_stand(wstinput::Controller const& controller);
  void update_stand_to_listen(wstinput::Controller const& controller);
  void update_turnaround(wstinput::Controller const& controller);
  void update_walk(wstinput::Controller const& controller);
  void update_walk_stand(wstinput::Controller const& controller);
  void update_jump_air(wstinput::Controller const& controller);
  void update_jump_begin(wstinput::Controller const& controller);
  void update_jump_land(wstinput::Controller const& controller);
  void update_jump_up_air(wstinput::Controller const& controller);
  void update_jump_up_begin(wstinput::Controller const& controller);
  void update_jump_up_land(wstinput::Controller const& controller);
  void update_pull_gun(wstinput::Controller const& controller);
  void update_stairs(wstinput::Controller const& controller, float delta);

  void leave_run();
  void leave_walk();
  void set_ducked();
  void set_ducking();
  void set_listen();
  void set_run();
  void set_stand();
  void set_stand_to_listen(bool backwards);
  void set_turnaround();
  void set_walk(Direction direction);

  void set_jump_air();
  void set_jump_begin();
  void set_jump_land();

  void set_jump_up_air();
  void set_jump_up_begin();
  void set_jump_up_land();

  /**
   * Sets an action for the sprite. In contrast to sprite->set_action this
   * function will not restart the action if it was already running
   */
  void try_set_action(std::string const& name, float speed = 1.0);

  Direction get_direction() const;

private:
  Player(Player const&);
  Player& operator=(Player const&);
};

} // namespace windstille

#endif

/* EOF */
