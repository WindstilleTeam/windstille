/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_LASER_POINTER_HPP
#define HEADER_WINDSTILLE_LASER_POINTER_HPP

#include "sprite2d/sprite.hpp"
#include "display/texture.hpp"
#include "game_object.hpp"

/** Simple class that generates a laser for pointing at objects */
class LaserPointer : public GameObject
{
private:
  Texture noise;
  Sprite  laserpointer;
  Sprite  laserpointer_light;
  float   progress;
  float   angle;

public:
  LaserPointer();
  ~LaserPointer();

  void draw(SceneContext& sc);
  void update(float delta);

  float get_angle() const;
  void  set_angle(float angle);
private:
  LaserPointer (const LaserPointer&);
  LaserPointer& operator= (const LaserPointer&);
};

#endif

/* EOF */
