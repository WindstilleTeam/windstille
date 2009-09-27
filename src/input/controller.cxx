//  $Id: controller.cxx,v 1.4 2003/10/31 23:24:41 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "../assert.hxx"
#include "controller.hxx"

Controller::Controller()
{
  buttons.resize(ControllerDef::get_button_count());
  axes.resize(ControllerDef::get_axis_count());
}

float
Controller::get_axis_state(int name) const
{
  AssertMsg(name < int(axes.size()), "Controllor: Unknown AxisName");  
  return axes[name];
}
        

bool
Controller::get_button_state(int name) const
{
  AssertMsg(name < int(buttons.size()), "Controllor: Unknown ButtonName");  
  return buttons[name];
}

void
Controller::set_axis_state(int name, float pos)
{
  AssertMsg(name < static_cast<int>(axes.size()), "Controllor: Unknown AxisName");
  axes[name] = pos;
}

void
Controller::set_button_state(int name, bool down)
{
  AssertMsg(name < static_cast<int>(buttons.size()), "Controller: Unknown ButtonName");
  buttons[name] = down;
}

void
Controller::add_axis_event(int name, float pos)
{
  InputEvent event;
  event.type = AXIS_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;
  events.push_back(event);
}

void
Controller::add_button_event(int name, bool down)
{
  InputEvent event;
  event.type = BUTTON_EVENT;
  event.button.name = name;
  event.button.down = down;
  events.push_back(event);
}

InputEventLst
Controller::get_events() const
{
  return events;
}

void
Controller::set_events(const InputEventLst& lst)
{
  events = lst;
}

/* EOF */
