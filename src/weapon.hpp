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

#ifndef WEAPON_HPP
#define WEAPON_HPP

/**
 * Base class for all weapons. This is not a GameObject because it will be
 * attached and handled by the Player object, not by the sector
 */
class Weapon
{
public:
  virtual ~Weapon()
  {}
  
  virtual void draw(SceneContext& context) = 0;
  virtual void update(float delta) = 0;

  virtual void fire(bool enable) = 0;
};

#endif

/* EOF */
