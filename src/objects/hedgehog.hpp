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

#ifndef HEADER_WINDSTILLE_OBJECTS_HEDGEHOG_HPP
#define HEADER_WINDSTILLE_OBJECTS_HEDGEHOG_HPP

#include "engine/game_object.hpp"
#include "engine/entity.hpp"

#include "sprite2d/sprite.hpp"

namespace windstille {

class Hedgehog : public Entity
{
private:
  Sprite sprite;
  Sprite die_sprite;
  Sprite light;
  Sprite highlight;

  bool direction_left;
  enum { WALKING, FALLING, DYING } state;
public:
  Hedgehog(ReaderMapping const& props);
  ~Hedgehog() override;

  void draw(wstdisplay::SceneContext& gc) override;
  void update(float delta) override;
  void die();
};

} // namespace windstille

#endif

/* EOF */
