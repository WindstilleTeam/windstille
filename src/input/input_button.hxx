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

#ifndef HEADER_INPUT_BOTTON_HXX
#define HEADER_INPUT_BOTTON_HXX

#include <vector>
#include <ClanLib/Signals/slot.h>
#include <ClanLib/Signals/signal_v0.h>

class InputButton
{
protected:
  std::vector<CL_Slot> slots;
  CL_Signal_v0 button_down;
  CL_Signal_v0 button_up;

public:
  InputButton() {}
  virtual ~InputButton() {}
  
  virtual void update(float delta) {}

  CL_Signal_v0& on_key_down() { return button_down; }
  CL_Signal_v0& on_key_up()   { return button_up; }
};

#endif

/* EOF */
