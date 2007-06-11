/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "globals.hpp"
#include "player.hpp"
#include "energy_bar.hpp"

EnergyBar::EnergyBar()
  : bar("images/energy_bar.sprite")
{
}

EnergyBar::~EnergyBar()
{
}
  
void
EnergyBar::draw()
{
  int energy     = Player::current()->get_energy();
  int max_energy = Player::current()->get_max_energy();

  for(int i = 0; i < energy; ++i)
    {
      float red   = 1.0f;
      float green = (i/float(max_energy));
      Sprite sprite = bar;
      sprite.set_color(Color(red, green, 0, 1.0f));
      sprite.draw(Vector(15 + (i * 10), 15));
    }

  for(int i = energy; i < max_energy; ++i)
    {
      bar.set_color(Color(.5f, .5f, .5f, .5f));
      bar.draw(Vector(15 + (i * 10), 15));
    }
}

void
EnergyBar::update(float delta, const Controller& )
{
  bar.update(delta);
}

/* EOF */