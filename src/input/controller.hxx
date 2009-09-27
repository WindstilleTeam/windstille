//  $Id: controller.hxx,v 1.4 2003/10/31 23:24:41 grumbel Exp $
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

#ifndef HEADER_CONTROLLER_HXX
#define HEADER_CONTROLLER_HXX

#include <vector>
#include "../controller_def.hxx"
#include "input_event.hxx"

/** The Controller class presents the current state of the controller
    and the input events that occurred on the controller since the
    last update */
class Controller
{
private:
  /** State of all buttons, indexed by ButtonName */
  std::vector<bool> buttons;
  
  /** State of all axis, indexed by AxisName */
  std::vector<float> axes;

  InputEventLst events;

public:
  Controller();

  float get_axis_state  (int name) const;
  bool  get_button_state(int name) const;

  void  set_axis_state  (int name, float pos);
  void  set_button_state(int name, bool down);

  void add_axis_event  (int name, float pos);
  void add_button_event(int name, bool down);

  InputEventLst get_events() const;
  void set_events(const InputEventLst& lst);
};

#endif

/* EOF */
