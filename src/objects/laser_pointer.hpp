/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_LASER_POINTER_HPP
#define HEADER_WINDSTILLE_OBJECTS_LASER_POINTER_HPP

#include "sprite2d/sprite.hpp"
#include <wstdisplay/texture.hpp>
#include "engine/game_object.hpp"

/** Simple class that generates a laser for pointing at objects */
class LaserPointer : public GameObject
{
private:
  wstdisplay::TexturePtr noise;
  Sprite  laserpointer;
  Sprite  laserpointer_light;
  float   progress;
  float   angle;

public:
  LaserPointer();
  ~LaserPointer() override;

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;

  float get_angle() const;
  void  set_angle(float angle);

private:
  LaserPointer (const LaserPointer&);
  LaserPointer& operator= (const LaserPointer&);
};

#endif

/* EOF */
