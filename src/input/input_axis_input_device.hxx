//  $Id$
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

#ifndef HEADER_INPUT_AXIS_INPUT_DEVICE_HXX
#define HEADER_INPUT_AXIS_INPUT_DEVICE_HXX

#include <ClanLib/Display/input_device.h>
#include "input_axis.hxx"

class InputAxisInputDevice : public InputAxis
{
private:
  CL_InputDevice dev;
  int axis_num;

  void on_axis_move(const CL_InputEvent& event); 

public:
  InputAxisInputDevice(CL_InputDevice& dev, int num);
  void update(float delta) {}
};

#endif

/* EOF */
