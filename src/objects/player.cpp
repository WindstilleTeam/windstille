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

#include <boost/bind.hpp>
#include "tile/tile_map.hpp"
#include "engine/sector.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "controller_def.hpp"
#include "player.hpp"
#include "objects/bomb.hpp"
#include "globals.hpp"
#include "pda.hpp"
#include "tile/tile.hpp"
#include "sprite3d/manager.hpp"
#include "display/surface_manager.hpp"
#include "particles/particle_system.hpp"
#include "sound/sound_manager.hpp"
#include "collision/collision_engine.hpp"
#include "laser_pointer.hpp"
#include "collision/stair_contact.hpp"
#include "game_session.hpp"
#include "console.hpp"
#include "grenade.hpp"
#include "pistol.hpp"

static const int MAX_ENERGY = 16;
static const float WALK_SPEED = 128.0;
static const float RUN_SPEED = 256.0;

Player* Player::current_ = 0;

Player::Player () :
  light("images/light3.sprite"),
  flashlight("images/flashlightlight.sprite"),
  flashlighthighlight("images/flashlighthighlight.sprite"),
  state(STAND)
{
  //laser_pointer = new LaserPointer();
  sprite = Sprite3D("models/characters/jane/jane.wsprite");
  pos.x = 320;
  pos.y = 200;
  name = "player";

  jumping = false;
  energy = MAX_ENERGY;
  current_ = this;

  hit_count = 0.0f;
  sprite.set_action("Stand");

  // collision detection init
  c_object = new CollisionObject(this, Rectf(-15, -120, 15, 0));

  c_object->set_pos(pos);
  c_object->set_velocity(velocity);
  
  c_object->sig_collision().connect(boost::bind(&Player::collision, this, _1));

  Sector::current()->get_collision_engine()->add(c_object);

  z_pos = 100.0f;

  contact = 0;
  weapon = new Pistol();
  laser_pointer = ((Pistol*) weapon)->laser_pointer;
}

Player::~Player()
{
  //delete laser_pointer;
}

void
Player::draw (SceneContext& sc)
{
  //light.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  //light.set_scale(4.0f);

  flashlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  flashlight.set_scale(2.0f);

  flashlighthighlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  flashlighthighlight.set_scale(2.0f);

  if (1)
    { // draw the 'stand-on' tile
      sc.highlight().fill_rect(Rect(Point(int(pos.x)/32 * 32, (int(pos.y)/32 + 1) * 32),
                                    Size(32, 32)),
                               Color(1.0f, 0.0f, 0.0f, 0.5f), 10000.0f);
    }

  //sc.highlight().draw(flashlighthighlight, pos - Vector(40, 80), 100.0f);
  //sc.light().draw(flashlight, pos - Vector(40, 80), 100.0f);
  sprite.draw(sc.color(), pos, z_pos);

  Entity* obj = find_useable_entity();
  if (obj)
    {
      std::string use_str = "[" + obj->get_use_verb() + "]";
      sc.highlight().draw(use_str, obj->get_pos().x, obj->get_pos().y - 150, 1000);
    }
  
  // Draw weapon at the 'Weapon' attachment point
  Sprite3D::PointID id = sprite.get_attachment_point_id("Weapon");
  sc.push_modelview();
  sc.translate(pos.x, pos.y);
  sc.mult_modelview(sprite.get_attachment_point_matrix(id));
  weapon->draw(sc);
  sc.pop_modelview();


  sc.push_modelview();
  sc.translate(pos.x, pos.y - 80);
  laser_pointer->draw(sc);
  sc.pop_modelview();
}

void
Player::start_listening()
{
  if(state == STAND_TO_LISTEN || state == LISTEN)
    return;

  set_stand_to_listen(false);
}

void
Player::stop_listening()
{
  if(state != LISTEN && state != STAND_TO_LISTEN)
    return;
  
  set_stand_to_listen(true);
}

void 
Player::update(float delta)
{
  weapon->update(delta);
  laser_pointer->update(delta);

  controller = InputManager::get_controller();

  laser_pointer->set_angle(laser_pointer->get_angle() + controller.get_axis_state(Y_AXIS) * delta);
  //laser_pointer->set_angle(controller.get_axis_state(Y2_AXIS) * M_PI);

  //std::cout << controller.get_axis_state(Y2_AXIS) * M_PI << std::endl;

  if (GameSession::current()->is_active())
    {
      switch(state) {
        case STAND:
        case WALK:
          update_walk_stand();
          break;
        case RUN:
          update_run();
          break;
        case DUCKING:
          update_ducking();
          break;
        case DUCKED:
          update_ducked();
          break;
        case TURNAROUND:
          update_turnaround();
          break;
        case STAND_TO_LISTEN:
          update_stand_to_listen();
          break;
        case LISTEN:
          update_listen();
          break;
        case JUMP_BEGIN:
          update_jump_begin();
          break;
        case JUMP_AIR:
          update_jump_air();
          break;
        case JUMP_LAND:
          update_jump_land();
          break;
        case JUMP_UP_BEGIN:
          update_jump_up_begin();
          break;
        case JUMP_UP_AIR:
          update_jump_up_air();
          break;
        case JUMP_UP_LAND:
          update_jump_up_land();
          break;
        case PULL_GUN:
          update_pull_gun();
          break;
        case STAIRS_DOWN:
        case STAIRS_UP:
          update_stairs(delta);
          break;
        default:
          assert(false);
          break;
      }
    }

  // fall down
  velocity.y += GRAVITY * delta;

  sprite.update(delta);

  c_object->set_velocity (velocity);

  pos = c_object->get_pos();
}

void
Player::set_stand()
{
  try_set_action("Stand");
  velocity = Vector(0, 0);
  state = STAND;
}

void
Player::update_walk_stand()
{
  if (controller.get_axis_state(Y_AXIS) > 0.5f) {
    TileMap* tilemap = Sector::current()->get_tilemap2();
    if (tilemap)
      {
        Point p(int(pos.x)/32, (int(pos.y)/32 + 1));
        unsigned int col = tilemap->get_pixel(p.x, p.y);

        if ((col & TILE_STAIRS) && (get_direction() == WEST && (col & TILE_LEFT) ||
                                    get_direction() == EAST && (col & TILE_RIGHT)))
          {
            delete contact;
            contact = new StairContact(tilemap, p);

            std::cout << "Stair mode" << std::endl;
            state = STAIRS_DOWN;
            //c_object->get_check_domains() & (~CollisionObject::DOMAIN_TILEMAP));
            Sector::current()->get_collision_engine()->remove(c_object);
            z_pos = -10.0f;
            return;
          }
        else
          {
            set_ducking();
            return;
          }
      }
  } else if (controller.get_axis_state(Y_AXIS) < -0.5f) {
    TileMap* tilemap = Sector::current()->get_tilemap2();
    if (tilemap)
      {
        Point p(int(pos.x)/32 + ((get_direction() == WEST) ? -1 : +1), (int(pos.y)/32));
        unsigned int col = tilemap->get_pixel(p.x, p.y);

        if ((col & TILE_STAIRS) && (get_direction() == EAST && (col & TILE_LEFT) ||
                                    get_direction() == WEST && (col & TILE_RIGHT)))
          {
            delete contact;
            contact = new StairContact(tilemap, p);

            state = STAIRS_UP;
            //c_object->get_check_domains() & (~CollisionObject::DOMAIN_TILEMAP));
            Sector::current()->get_collision_engine()->remove(c_object);
            z_pos = -10.0f;
            return;
          }
      }    
  }

  if(state == STAND)
    update_stand();
  else
    update_walk();
}

void
Player::update_stairs(float delta)
{
  assert(contact);

  if (controller.get_axis_state(X_AXIS) < -0.5f ||
      controller.get_axis_state(Y_AXIS) > 0.5f)
    contact->advance(-WALK_SPEED * delta * 0.7f);
  else if (controller.get_axis_state(X_AXIS) > 0.5f ||
           controller.get_axis_state(Y_AXIS) < -0.5f)
    contact->advance(WALK_SPEED * delta * 0.7f);

  velocity = Vector(0, 0);
  c_object->set_pos(contact->get_pos());

  if (!contact->is_active())
    {
      Sector::current()->get_collision_engine()->add(c_object);
      state = STAND;
    }
}

Entity*
Player::find_useable_entity()
{
  std::vector<GameObject*>* objects = Sector::current()->get_objects();
  for (std::vector<GameObject*>::iterator i = objects->begin(); i != objects->end(); ++i)
    {
      Entity* object = dynamic_cast<Entity*>(*i);
      if (object && object != this && object->is_useable())
        {
          //FIXME use proper collision detection
          if (object->get_pos().x > pos.x - 32 && object->get_pos().x < pos.x + 32
              && object->get_pos().y > pos.y - 256 && object->get_pos().y < pos.y + 256)
            {
              return object;
            }
        }
    }
  return 0;  
}


void
Player::update_stand()
{ 
  if(controller.button_was_pressed(USE_BUTTON))
    {
      Entity* obj = find_useable_entity();
      if (obj)
        obj->use();
      return;
    }
    
  if(controller.button_was_pressed(JUMP_BUTTON)
     && controller.get_axis_state(Y_AXIS) > 0.5f) 
    {
      set_jump_up_begin();
    } 
  else if (controller.button_was_pressed(AIM_BUTTON))
    {
      // TODO remove me later, just here for testing
      Grenade* grenade = new Grenade();
      grenade->set_pos(get_pos() + Vector(50, -300));
      grenade->set_velocity(Vector(20, -10));
      Sector::current()->add(grenade);
      
      sprite.set_action("PullGun");
      state = PULL_GUN;
    }
  else if (controller.get_axis_state(X_AXIS) < -0.5f) 
    {
      if(get_direction() == WEST)
        set_walk(WEST);
      else
        set_turnaround();
    }
  else if (controller.get_axis_state(X_AXIS) > 0.5f) 
    {
      if(get_direction() == EAST)
        set_walk(EAST);
      else
        set_turnaround();
    }
}

void
Player::set_walk(Direction direction)
{
  try_set_action("Walk");
  sprite.set_rot(direction == EAST);
  state = WALK;
  if(direction == EAST)
    velocity.x = WALK_SPEED;
  else
    velocity.x = -WALK_SPEED;

  // start walking sound
  // sound_source.reset(sound_manager->create_sound_source("sounds/steps_dirt.ogg"));
  // sound_source->set_looping(true);
  // sound_source->play();
}

void
Player::update_walk()
{
  if (controller.get_axis_state(X_AXIS) == 0) {
    leave_walk();
    set_stand();
    return;
  }

  if(get_direction() == WEST && controller.get_axis_state(X_AXIS) > 0.5f
     || get_direction() == EAST && controller.get_axis_state(X_AXIS) < -0.5f) {
    leave_walk();
    set_turnaround();
    return;
  }
  
  if(controller.get_button_state(RUN_BUTTON)) {
    leave_walk();
    set_run();
    return;
  }
}

void
Player::leave_walk()
{
  // stop walking sound
  // sound_source->stop();
}

void
Player::set_ducking()
{
  try_set_action("StandToDuck");
  sprite.set_next_action("Ducking");
  state = DUCKING;
  velocity.x = 0;
}

void
Player::update_ducking()
{
  // ducking
  if(sprite.switched_actions()) {
    if(sprite.get_action() == "Ducking")
      set_ducked();
    else
      set_stand();
    return;
  }
  
  if(!(controller.get_axis_state(Y_AXIS) > 0.5f) && sprite.get_speed() > 0) {
    sprite.set_speed(-1.0);
    sprite.set_next_action("Stand");
    state = STAND;
  } else if(controller.get_axis_state(Y_AXIS) > 0.5f && sprite.get_speed() < 0) {
    sprite.set_speed(1.0);
    sprite.set_next_action("Ducking");
  }
}

void
Player::set_ducked()
{
  assert(sprite.get_action() == "Ducking");
  state = DUCKED;
}

void
Player::update_ducked()
{
  if(!controller.get_axis_state(Y_AXIS) > 0.5f) {
    state = DUCKING;
    sprite.set_action("StandToDuck", -1.0);
    sprite.set_next_action("Stand");
  }  
}

void
Player::set_turnaround()
{
  velocity.x = 0;
  try_set_action("Turn");
  sprite.set_next_action("Walk");
  sprite.set_next_rot(! sprite.get_rot());
  state = TURNAROUND;
}

void
Player::update_turnaround()
{
  if(sprite.switched_actions()) {
    if(sprite.get_rot()) {
      set_walk(EAST);
    } else {
      set_walk(WEST);
    }
  } 
  if(sprite.get_rot() && controller.get_axis_state(X_AXIS) > 0.5f
     || !sprite.get_rot() && controller.get_axis_state(X_AXIS) < -0.5f) {
    sprite.set_speed(-1.0);
    sprite.set_next_action("Walk");
    state = WALK;
  }
}

void
Player::set_stand_to_listen(bool backwards)
{
  try_set_action("StandtoListen", backwards ? -1.0 : 1.0);
  if(!backwards) {
    sprite.set_next_action("Listen");
    velocity = Vector(0, 0);
  } else {
    sprite.set_next_action("Stand");
  }
  state = STAND_TO_LISTEN;
}

void
Player::update_stand_to_listen()
{
  if(sprite.switched_actions()) {
    if(sprite.get_action() == "Stand")
      set_stand();
    else
      set_listen();
  }
}

void
Player::set_listen()
{
  try_set_action("Listen");
  state = LISTEN;
}

void
Player::update_listen()
{
  // nothing
}

void
Player::set_run()
{
  try_set_action("Run");
  if(get_direction() == EAST)
    velocity.x = RUN_SPEED;
  else
    velocity.x = -RUN_SPEED;  
  state = RUN;

  // start running sound
  // sound_source.reset(sound_manager->create_sound_source("sounds/steps_dirt.ogg"));
  // sound_source->set_looping(true);
  // sound_source->play();
}

void
Player::update_run()
{
  if(!controller.get_button_state(RUN_BUTTON)) {
    leave_run();
    set_walk(get_direction());
    return;
  }
  if(controller.get_button_state(JUMP_BUTTON)) {
    leave_run();
    set_jump_begin();
    return;
  }
}

void
Player::leave_run()
{
  // sound_source->stop();
}

void
Player::set_jump_begin()
{
  if(sprite.before_marker("RightFoot")) {
    sprite.set_next_action("JumpRightFoot");
    sprite.abort_at_marker("RightFoot");
    jump_foot = LEFT_FOOT;
  } else if(sprite.before_marker("LeftFoot")) {
    sprite.set_next_action("JumpLeftFoot");
    sprite.abort_at_marker("LeftFoot");
    jump_foot = RIGHT_FOOT;
  } else {
    sprite.set_next_action("JumpRightFoot");
    sprite.abort_at_marker("RightFoot");
    jump_foot = LEFT_FOOT;
  }
  state = JUMP_BEGIN;
}

void
Player::update_jump_begin()
{
  if(sprite.switched_actions()) {
    if(sprite.get_action() == "JumpLeftFoot") {
      sprite.set_next_action("JumpLeftFootAir");
    } else if(sprite.get_action() == "JumpRightFoot") {
      sprite.set_next_action("JumpRightFootAir");
    } else {
      set_jump_air();
      return;
    }
  }
}

void
Player::set_jump_air()
{
  velocity.y = -450;
  sprite.set_next_action("JumpLandSofttoRun");
  state = JUMP_AIR;
}

void
Player::update_jump_air()
{
  if(sprite.switched_actions()) {
    set_jump_land();
    return;
  }
}

void
Player::set_jump_land()
{
  sprite.set_next_action("Run");
  state = JUMP_LAND;
}

void
Player::update_jump_land()
{
  if(sprite.switched_actions()) {
    set_run();
    return;
  }
}

void
Player::set_jump_up_begin()
{
  sprite.set_next_action("JumpUp");
  state = JUMP_UP_BEGIN;
}

void
Player::update_jump_up_begin()
{
  if(sprite.switched_actions()) {
    set_jump_up_air();
    return;
  }
}

void
Player::set_jump_up_air()
{
  velocity.y = -400;
  sprite.set_next_action("JumpLandSofttoRun");
  state = JUMP_UP_AIR;
}

void
Player::update_jump_up_air()
{
  if(sprite.switched_actions()) {
    set_jump_up_land();
    return;
  }
}

void
Player::set_jump_up_land()
{
  sprite.set_next_action("Stand");
  state = JUMP_UP_LAND;
}

void
Player::update_jump_up_land()
{
  if(sprite.switched_actions()) {
    set_stand();
    return;
  }
}

void
Player::update_pull_gun()
{
  if (!controller.get_button_state(AIM_BUTTON))
    {
      sprite.set_next_action("Stand");      
      state = STAND;
    }
}

Direction
Player::get_direction() const
{
  return sprite.get_rot() ? EAST : WEST;
}

void
Player::try_set_action(const std::string& name, float speed)
{
  if(sprite.get_action() == name)
    return;
  
  sprite.set_action(name, speed);
}

int
Player::get_energy() const
{
  return energy;
}

int
Player::get_max_energy() const
{
  return MAX_ENERGY;
}

void
Player::hit(int points)
{
  if (energy > 0 && hit_count <= 0)
    {
      energy -= points;
      //hit_count = 1.0f;

      if (energy <= 0)
        {
          //switch_movement_state(KILLED);
          hit_count = 0;
        }
    }
}

void
Player::collision(const CollisionData& data)
{
  // copy velocity, as "velocity" is the wanted velocity, whereas
  // cur_vel is the velocity in the current delta-frame
  Vector cur_vel = c_object->get_velocity(); 
  if (data.direction.y != 0)
    {
      cur_vel.y = 0;
      velocity.y = 0; // also reset vertical velocity
    }
  else
    {
      // do not reset horizontal velocity, as it's only set, when starting to go/run
      cur_vel.x = 0;
    }
  c_object->set_velocity (cur_vel);
  
}

void 
Player::set_pos(Vector pos)
{
  Entity::set_pos(pos);
  c_object->set_pos(pos);
}

/* EOF */
