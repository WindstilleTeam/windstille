/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "objects/laser_pointer.hpp"
#include "objects/pistol.hpp"

Pistol::Pistol() :
  sprite(Pathname("models/objects/pistol/pistol.wsprite")),
  laser_pointer(new LaserPointer())
{
}

Pistol::~Pistol()
{
  delete laser_pointer;
}

void
Pistol::draw(SceneContext& sc)
{
  sprite.draw(sc.color(), Vector2f(0, 0), 1000);
  // Disabled for now and done in the player class
  //if(laser_pointer)
  //  laser_pointer->draw(sc);
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
