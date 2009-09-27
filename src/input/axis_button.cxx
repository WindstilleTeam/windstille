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

#include <iostream>
#include "input_axis.hxx"
#include "axis_button.hxx"

AxisButton::AxisButton(InputAxis* axis, bool top)
  : axis(axis),
    down(false),
    top(top)
{
  on_axis_move_slot = axis->on_move().connect(this, &AxisButton::on_axis_move);
}

AxisButton::~AxisButton()
{
}

void
AxisButton::update(float delta)
{
}

void
AxisButton::on_axis_move(float pos)
{
  if (top)
    {
      if (down && pos < 0.5f)
        {
          down = false;
          button_up();
        }
      else if (!down && pos > 0.5f)
        {
          down = true;
          button_down();
        }
    }
  else
    {
      if (down && pos > -0.5f)
        {
          down = false;
          button_up();
        }
      else if (!down && pos < -0.5f)
        {
          down = true;
          button_down();
        }
    }
}

/* EOF */
