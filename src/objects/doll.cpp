/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "objects/doll.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/projection.hpp>

#include <geom/line.hpp>
#include <wstinput/controller.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "engine/sector.hpp"
#include "navigation/edge.hpp"
#include "navigation/edge_position.hpp"
#include "navigation/navigation_graph.hpp"
#include "navigation/node.hpp"
#include <wstdisplay/scenegraph/scene_graph.hpp>
#include "sprite3d/sprite3d_drawable.hpp"
#include "sprite3d/manager.hpp"

namespace windstille {

Doll::Doll() :
  m_drawable(),
  m_velocity(),
  m_pos(200, 600),
  m_last_pos(m_pos),
  m_edge_position(),
  m_state(kNoState)
{
  Sprite3D sprite = g_app.sprite3d().create(Pathname("models/characters/jane/jane.wsprite"));
  m_drawable.reset(new Sprite3DDrawable(sprite, m_pos, 100.0f, glm::mat4(1.0f)));
  m_drawable->set_scale(2.0f);

  Sector::current()->get_scene_graph().add_drawable(m_drawable);

  set_state_falling();
}

Doll::~Doll()
{
}

void
Doll::draw (wstdisplay::SceneContext& sc)
{
  sc.highlight().fill_rect(geom::frect(m_pos - glm::vec2(-10.0f, -10.0f),
                                       m_pos + glm::vec2(-10.0f, -10.0f)),
                           surf::Color(1.0f, 1.0f, 1.0f));
}

void
Doll::update (float /*delta*/)
{
}

void
Doll::update(wstinput::Controller const& controller, float delta)
{
  glm::vec2 stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  switch(m_state)
  {
    case kFalling:  update_falling(controller, delta);  break;
    case kWalking:  update_walking(controller, delta);  break;
    case kRunning:  update_running(controller, delta);  break;
    case kStanding: update_standing(controller, delta); break;
    case kDucking:  update_ducking(controller, delta);  break;
    case kJumpUp:   update_jump_up(controller, delta);  break;
    case kRolling:  update_rolling(controller, delta);  break;
    case kListing:  update_listing(controller, delta);  break;
    case kSwinging: update_swinging(controller, delta);  break;
    case kClimbing: update_climbing(controller, delta);  break;
    case kNoState:  break;
  }

  m_drawable->get_sprite().update(delta);
  m_drawable->set_pos(m_pos);
  m_last_pos = m_pos;
}

void
Doll::set_state_jump_up()
{
  if (m_state != kJumpUp)
  {
    m_state = kJumpUp;
    m_drawable->get_sprite().set_action("JumpUp");
    m_drawable->get_sprite().set_next_action("Stand");
  }
}

void
Doll::set_state_falling()
{
  if (m_state != kFalling)
  {
    m_state = kFalling;
  }
}

void
Doll::set_state_standing()
{
  if (m_state != kStanding)
  {
    m_state = kStanding;
    m_drawable->get_sprite().set_action("Stand");
  }
}

void
Doll::set_state_walking()
{
  if (m_state != kWalking)
  {
    m_state = kWalking;
    m_drawable->get_sprite().set_action("Walk");
  }
}

void
Doll::set_state_running()
{
  if (m_state != kRunning)
  {
    m_state = kRunning;
    m_drawable->get_sprite().set_action("Run");
  }
}

void
Doll::set_state_ducking()
{
  if (m_state != kDucking)
  {
    m_state = kDucking;
    m_drawable->get_sprite().set_action("StandToDuck");
    m_drawable->get_sprite().set_next_action("Ducking");
  }
}

void
Doll::set_state_rolling()
{
  if (m_state != kRolling)
  {
    m_drawable->get_sprite().set_action("Roll");

    switch(m_state)
    {
      case kStanding: m_drawable->get_sprite().set_next_action("Stand"); break;
      case kWalking:  m_drawable->get_sprite().set_next_action("Walk"); break;
      case kRunning:  m_drawable->get_sprite().set_next_action("Run"); break;
      default:        m_drawable->get_sprite().set_next_action("Run"); break;
    }

    m_state = kRolling;
  }
}

void
Doll::set_state_listing()
{
  if (m_state != kListing)
  {
    m_state = kListing;
    m_drawable->get_sprite().set_action("Listen");
  }
}

void
Doll::set_state_swinging()
{
  if (m_state != kSwinging)
  {
    m_state = kSwinging;
    m_drawable->get_sprite().set_action("Switching");
  }
}

void
Doll::set_state_climbing()
{
  if (m_state != kClimbing)
  {
    m_state = kClimbing;
    m_drawable->get_sprite().set_action("Climb");
  }
}

void
Doll::set_state_jump()
{

}


void
Doll::update_falling(wstinput::Controller const& controller, float delta)
{
  glm::vec2 stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  m_pos += stick;
  m_pos.y += 1000.0f * delta;

  std::vector<EdgePosition> const& intersections =
    Sector::current()->get_navigation_graph().find_intersections(geom::fline(m_last_pos, m_pos));

  if (!intersections.empty())
  {
    std::cout << "Attaching to navgraph" << std::endl;
    m_edge_position.reset(new EdgePosition(intersections.front()));
    set_state_standing();
  }
}

void
Doll::update_standing(wstinput::Controller const& controller, float /*delta*/)
{
  if (controller.get_axis_state(Y_AXIS) > 0)
  {
    set_state_ducking();
  }
  else if (controller.button_was_pressed(JUMP_BUTTON))
  {
    std::cout << "Jump Up" << std::endl;
    set_state_jump_up();
  }
  else if (controller.get_axis_state(X_AXIS) > 0 ||
           controller.get_axis_state(X_AXIS) < 0)
  {
    if (controller.get_button_state(RUN_BUTTON))
    {
      set_state_running();
    }
    else
    {
      set_state_walking();
    }
  }
}

void
Doll::update_walking(wstinput::Controller const& controller, float delta)
{
  glm::vec2 stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);
  walk(stick);

  if (controller.get_axis_state(Y_AXIS) > 0)
  {
    set_state_rolling();
  }
  else
  {
    if (controller.get_axis_state(X_AXIS) == 0)
    {
      set_state_standing();
    }
    else if (controller.get_button_state(RUN_BUTTON))
    {
      set_state_running();
    }
  }
}

void
Doll::update_running(wstinput::Controller const& controller, float delta)
{
  glm::vec2 stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  walk(stick * 2.5f);

  if (controller.get_axis_state(Y_AXIS) > 0)
  {
    set_state_rolling();
  }
  else
  {
    if (controller.get_axis_state(X_AXIS) == 0)
    {
      set_state_standing();
    }
    else if (!controller.get_button_state(RUN_BUTTON))
    {
      set_state_walking();
    }
  }
}

void
Doll::update_ducking(wstinput::Controller const& controller, float /*delta*/)
{
  if (controller.get_axis_state(Y_AXIS) < 0)
  {
    set_state_standing();
  }
  else if (controller.get_axis_state(X_AXIS) > 0 ||
           controller.get_axis_state(X_AXIS) < 0)
  {
    if (controller.get_button_state(RUN_BUTTON))
    {
      set_state_running();
    }
    else
    {
      set_state_walking();
    }
  }
}

void
Doll::update_jump_up(wstinput::Controller const& controller, float /*delta*/)
{
  if (m_drawable->get_sprite().switched_actions())
  {
    set_state_standing();
  }
}

void
Doll::walk(glm::vec2 const& adv_)
{
  if (adv_.x > 0)
  {
    m_drawable->get_sprite().set_rot(true);
  }
  else if (adv_.x < 0)
  {
    m_drawable->get_sprite().set_rot(false);
  }

  if (m_edge_position)
  {
    Node* next_node = nullptr;
    glm::vec2 adv(adv_);
    m_edge_position->advance(adv, next_node);

    if (!(adv.x == 0.0f && adv.y == 0.0f))
    { // Not all advancement got used up, which means we have hit
      // the end of a edge

      // FIXME: This should be a while loop, currently we are just
      // discarding the rest movement

      EdgePosition next_edge;
      float length = 0;
      for(Node::Edges::iterator i = next_node->edges.begin(); i != next_node->edges.end(); ++i)
      {
        if (m_edge_position->get_edge() != i->edge)
        { // Find out into the direction of which edge the stick is pointing
          glm::vec2 proj = glm::proj(adv_, i->edge->get_vector());

          if (glm::length(proj) > length)
          {
            next_edge = *i;
            length    = glm::length(proj);
          }
        }
      }

      if (!next_edge.edge)
      {
        /*
          std::cout << "Dead End" << std::endl;
          m_edge_position.reset();

          // FIXME: Voodoo to fix connection/deadend cicles
          player += stick;
          old_player = player;
        */
      }
      else
      {
        //std::cout << "transition: " << next_edge << std::endl;
        *m_edge_position = next_edge;
      }
    }

    m_pos = m_edge_position->get_pos();
  }
}

void
Doll::update_rolling(wstinput::Controller const& controller, float delta)
{
  glm::vec2 stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  walk(stick * 2.5f);

  // FIXME: Need an Sprite::action_done() instead
  if (m_drawable->get_sprite().switched_actions())
  {
    if (controller.get_axis_state(X_AXIS) > 0 ||
        controller.get_axis_state(X_AXIS) < 0)
    {
      if (controller.get_button_state(RUN_BUTTON))
      {
        set_state_running();
      }
      else
      {
        set_state_walking();
      }
    }
    else
    {
      set_state_standing();
    }
  }
}

void
Doll::update_listing(wstinput::Controller const& controller, float /*delta*/)
{
}

void
Doll::update_swinging(wstinput::Controller const& controller, float /*delta*/)
{
}

void
Doll::update_climbing(wstinput::Controller const& controller, float /*delta*/)
{
}

void
Doll::update_jump(wstinput::Controller const& controller, float /*delta*/)
{
}

} // namespace windstille

/* EOF */
