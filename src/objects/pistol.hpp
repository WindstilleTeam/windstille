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

#ifndef HEADER_WINDSTILLE_OBJECTS_PISTOL_HPP
#define HEADER_WINDSTILLE_OBJECTS_PISTOL_HPP

#include "sprite3d/sprite3d.hpp"
#include "engine/weapon.hpp"

namespace windstille {

class LaserPointer;

class Pistol : public Weapon
{
private:
  Sprite3D sprite;
  //LaserPointer* laser_pointer;

public:
  LaserPointer* laser_pointer;

  Pistol();
  ~Pistol() override;

  void draw(wstdisplay::SceneContext& context) override;
  void update(float delta) override;

  void fire(bool enable) override;

private:
  Pistol(Pistol const&);
  Pistol& operator=(Pistol const&);
};

} // namespace windstille

#endif

/* EOF */
