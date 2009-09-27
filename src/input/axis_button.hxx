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

#ifndef HEADER_AXIS_BUTTON_HXX
#define HEADER_AXIS_BUTTON_HXX

#include "input_button.hxx"

class InputAxis;

/** */
class AxisButton : public InputButton
{
private:
  CL_Slot on_axis_move_slot;
  InputAxis* axis;
  bool down;

  /** true if the range 0..1 should be used to trigger, false if -1..0
      should be used */
  bool top;
public:
  AxisButton(InputAxis* axis, bool top);
  ~AxisButton();

  void update(float delta);
  
private:
  void on_axis_move(float pos);

  AxisButton (const AxisButton&);
  AxisButton& operator= (const AxisButton&);
};

#endif

/* EOF */
