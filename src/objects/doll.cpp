/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "scenegraph/sprite3d_drawable.hpp"
#include "scenegraph/scene_graph.hpp"
#include "engine/sector.hpp"
#include "input/controller.hpp"
#include "math/line.hpp"
#include "navigation/edge_position.hpp"
#include "navigation/edge.hpp"
#include "navigation/node.hpp"
#include "navigation/navigation_graph.hpp"

#include "objects/doll.hpp"

Doll::Doll()
  : m_drawable(),
    m_velocity(),
    m_pos(200, 600),
    m_last_pos(m_pos),
    m_edge_position(),
    m_state(kNoState)
{
  Sprite3D sprite(Pathname("models/characters/jane/jane.wsprite"));
  m_drawable.reset(new Sprite3DDrawable(sprite, m_pos, 100.0f, Matrix::identity()));
  m_drawable->set_scale(2.0f);

  Sector::current()->get_scene_graph().add_drawable(m_drawable);

  set_state_falling();
}

Doll::~Doll()
{
}

void
Doll::update (float /*delta*/)
{
}

void
Doll::update(const Controller& controller, float delta)
{
  Vector2f stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  switch(m_state)
  {
    case kFalling:  update_falling(controller, delta);  break;
    case kWalking:  update_walking(controller, delta);  break;
    case kRunning:  update_running(controller, delta);  break;
    case kStanding: update_standing(controller, delta); break;
    case kDucking:  update_ducking(controller, delta);  break;
    case kJumpUp:   update_jump_up(controller, delta);  break;
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
    m_drawable->get_sprite().set_next_action("JumpUp");
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
Doll::update_falling(const Controller& controller, float delta)
{
  Vector2f stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  m_pos += stick;
  m_pos.y += 1000.0f * delta;

  const std::vector<EdgePosition>& intersections =
    Sector::current()->get_navigation_graph().find_intersections(Line(m_last_pos, m_pos));

  if (!intersections.empty())
  {
    std::cout << "Attaching to navgraph" << std::endl;
    m_edge_position.reset(new EdgePosition(intersections.front()));
    set_state_standing();
  }
}

void
Doll::update_standing(const Controller& controller, float /*delta*/)
{
  if (controller.get_axis_state(Y_AXIS) > 0)
  {
    set_state_ducking();
  }
  else if (controller.get_button_state(JUMP_BUTTON))
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
Doll::update_walking(const Controller& controller, float delta)
{
  Vector2f stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);  
  walk(stick);

  if (controller.get_axis_state(X_AXIS) == 0)
  {
    set_state_standing();
  }
  else if (controller.get_button_state(RUN_BUTTON))
  {
    set_state_running();
  }
}

void
Doll::update_running(const Controller& controller, float delta)
{
  Vector2f stick(controller.get_axis_state(X_AXIS) * delta * 200.0f,
                 controller.get_axis_state(Y_AXIS) * delta * 200.0f);

  walk(stick * 2.5f);

  if (controller.get_axis_state(X_AXIS) == 0)
  {
    set_state_standing();
  }
  else if (!controller.get_button_state(RUN_BUTTON))
  {
    set_state_walking();
  }
}

void
Doll::update_ducking(const Controller& controller, float /*delta*/)
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
Doll::update_jump_up(const Controller& controller, float /*delta*/)
{
  if (!controller.get_button_state(JUMP_BUTTON))
  {
    set_state_standing();
  }  
}

void
Doll::walk(const Vector2f& adv_)
{
  if (adv_.x > 0)
  {
    m_drawable->get_sprite().set_rot(true);
  }
  else if (adv_.x < 0)
  {
    m_drawable->get_sprite().set_rot(false);
  }

  if (m_edge_position.get())
  {
    Node* next_node = 0;
    Vector2f adv(adv_);
    m_edge_position->advance(adv, next_node);

    if (!adv.is_null())
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
          Vector2f proj = adv_.project(i->edge->get_vector());
                  
          if (proj.length() > length)
          {
            next_edge = *i;
            length    = proj.length();
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

/* EOF */
