/*
**
**  Pingus - A free Lemmings clone
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

#include "input/controller.hpp"

#include <math.h>
#include <assert.h>

Controller::Controller() :
  m_states(LAST_EVENT),
  m_events() // FIXME: need to mark states with type
{
}

float
Controller::get_trigger_state(int name) const
{
  float value = get_axis_state(name)/2.0f + 0.5f;
  if (value < 0.001f)
  {
    return 0;
  }
  else if (value > 0.999f)
  {
    return 1.0f;
  }
  else
  {
    return value;
  }
}

float
Controller::get_axis_state(int id, bool use_deadzone) const
{
  assert(id < int(m_states.size()));

  if (use_deadzone)
  {
    if (fabsf(m_states[id].axis) > 0.25f) // FIXME: Hardcoded Deadzone
      return m_states[id].axis;
    else
      return 0.0f;
  }
  else
  {
    return m_states[id].axis;
  }
}

bool
Controller::get_button_state(int id) const
{
  assert(id < int(m_states.size()));
  return m_states[id].button;
}

void
Controller::set_axis_state(int id, float pos)
{
  assert(id < static_cast<int>(m_states.size()));
  m_states[id].axis = pos;
}

void
Controller::set_button_state(int name, bool down)
{
  assert(name < static_cast<int>(m_states.size()));
  m_states[name].button = down;
}

void
Controller::add_axis_event(int name, float pos)
{
  InputEvent event;

  event.type = AXIS_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;

  m_events.push_back(event);
}

void
Controller::add_button_event(int name, bool down)
{
  InputEvent event;

  event.type = BUTTON_EVENT;
  event.button.name = name;
  event.button.down = down;

  m_events.push_back(event);
}

const InputEventLst&
Controller::get_events() const
{
  return m_events;
}

void
Controller::set_events(const InputEventLst& lst)
{
  m_events = lst;
}

bool
Controller::button_was_pressed(int name) const
{
  for(InputEventLst::const_iterator i = m_events.begin(); i != m_events.end(); ++i)
  {
    if (i->type == BUTTON_EVENT && i->button.name == name && i->button.down)
    {
      return true;
    }
  }
  return false;
}

bool
Controller::axis_was_pressed_up(int name) const
{
  for(InputEventLst::const_iterator i = m_events.begin(); i != m_events.end(); ++i)
  {
    if (i->type == AXIS_EVENT && i->axis.name == name && i->axis.pos > 0.5f)
    {
      return true;
    }
  }
  return false;
}

bool
Controller::axis_was_pressed_down(int name) const
{
  for(InputEventLst::const_iterator i = m_events.begin(); i != m_events.end(); ++i)
  {
    if (i->type == AXIS_EVENT && i->axis.name == name && i->axis.pos < -0.5f)
    {
      return true;
    }
  }
  return false;
}

void
Controller::clear()
{
  m_events.clear();
  // FIXME: EVIL HARDCODE
  m_states[0].ball = 0;
  m_states[1].ball = 0;
}

void
Controller::add_event(const InputEvent& event)
{
  m_events.push_back(event);
}

float
Controller::get_ball_state(int id) const
{
  assert(id < int(m_states.size()));
  return m_states[id].ball;
}

void
Controller::set_ball_state(int id, float pos)
{
  assert(id < static_cast<int>(m_states.size()));
  m_states[id].ball = pos;
}

void
Controller::add_ball_event(int name, float pos)
{
  InputEvent event;

  event.type = BALL_EVENT;
  event.ball.name = name;
  event.ball.pos  = pos;

  m_events.push_back(event);
}

/* EOF */
