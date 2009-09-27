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
#include "input_button.hxx"
#include "button_axis.hxx"

ButtonAxis::ButtonAxis(InputButton* left, InputButton* right)
  : left(left), right(right)
{
  slots.push_back(left->on_key_down().connect(this, &ButtonAxis::on_left_down));
  slots.push_back(left->on_key_up().connect  (this, &ButtonAxis::on_left_up));

  slots.push_back(right->on_key_down().connect(this, &ButtonAxis::on_right_down));
  slots.push_back(right->on_key_up().connect  (this, &ButtonAxis::on_right_up));

  left_state  = false;
  right_state = false;
  
  pos = 0.0f;
}

void
ButtonAxis::update(float delta)
{
  left->update(delta);
  right->update(delta);

  float new_pos = 0.0f;
  if (left_state)
    {
      new_pos -= 1.0f;
    }
  
  if (right_state)
    {
      new_pos += 1.0f;
    }

  if (new_pos != pos)
    {
      pos = new_pos;
      move(pos);
    }
}

void
ButtonAxis::on_left_up()
{
  left_state = false;
}

void
ButtonAxis::on_left_down()
{
  left_state = true;
}

void
ButtonAxis::on_right_up()
{
  right_state = false;
}

void
ButtonAxis::on_right_down()
{
  right_state = true;
}

/* EOF */
