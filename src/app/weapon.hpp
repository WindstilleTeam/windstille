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

#ifndef HEADER_WINDSTILLE_APP_WEAPON_HPP
#define HEADER_WINDSTILLE_APP_WEAPON_HPP

namespace windstille {

/**
 * Base class for all weapons. This is not a GameObject because it will be
 * attached and handled by the Player object, not by the sector
 */
class Weapon
{
public:
  virtual ~Weapon()
  {}

  virtual void draw(wstdisplay::SceneContext& context) = 0;
  virtual void update(float delta) = 0;

  virtual void fire(bool enable) = 0;
};

} // namespace windstille

#endif

/* EOF */
