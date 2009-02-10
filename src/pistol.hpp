//
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

#ifndef PISTOL_HPP
#define PISTOL_HPP

#include "sprite3d/sprite3d.hpp"
#include "weapon.hpp"

class LaserPointer;

class Pistol : public Weapon
{
private:
  Sprite3D sprite;
  //LaserPointer* laser_pointer;

public:
  LaserPointer* laser_pointer;
  
  Pistol();
  virtual ~Pistol();
  
  void draw(SceneContext& context);
  void update(float delta);

  void fire(bool enable);
};

#endif

/* EOF */
