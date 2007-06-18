//  $Id$
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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


#include "laser_pointer.hpp"
#include "pistol.hpp"

Pistol::Pistol()
  : laser_pointer(0)
{
  laser_pointer = new LaserPointer();
  sprite = Sprite3D("models/objects/pistol/pistol.wsprite");
}

Pistol::~Pistol()
{
  delete laser_pointer;
}

void
Pistol::draw(SceneContext& sc)
{
  sprite.draw(sc.color(), Vector(0, 0), 1000);
  if(laser_pointer)
    laser_pointer->draw(sc);
}

void
Pistol::update(float delta)
{
  if(laser_pointer)
    laser_pointer->update(delta);

  sprite.update(delta);
}

void
Pistol::fire(bool )
{
}

/* EOF */
