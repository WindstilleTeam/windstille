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
#include "multi_button.hxx"

MultiButton::MultiButton()
{
  press_count = 0;
}

void
MultiButton::add(InputButton* button)
{
  buttons.push_back(button);
  slots.push_back(button->on_key_down().connect(this, &MultiButton::pressed));
  slots.push_back(button->on_key_up().connect(this, &MultiButton::released));
}

void
MultiButton::update(float delta)
{
  for (Buttons::iterator i = buttons.begin(); i != buttons.end(); ++i)
    (*i)->update(delta);
}

void
MultiButton::released()
{
  press_count -= 1;

  if (press_count == 0)
    button_up();

  // FIXME: Workaround
  if (press_count < 0)
    press_count = 0;
}

void
MultiButton::pressed()
{
  press_count += 1;

  if (press_count == 1)
    button_down();
}

/* EOF */
