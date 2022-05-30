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

#ifndef HEADER_WINDSTILLE_OBJECTS_BOMB_HPP
#define HEADER_WINDSTILLE_OBJECTS_BOMB_HPP

#include "sprite2d/sprite.hpp"
#include "engine/game_object.hpp"

namespace windstille {

/** */
class Bomb : public GameObject
{
private:
  Sprite sprite;
  Sprite explo;
  Sprite light;
  Sprite highlight;
  Sprite explolight;

  glm::vec2 pos;
  float count;
  enum { COUNTDOWN, EXPLODE } state;
  bool exploded;

public:
  Bomb(int x, int y);
  ~Bomb() override;

  void update(float delta) override;
  void draw(wstdisplay::SceneContext& gc) override;
private:
  void explode();
  Bomb (Bomb const&);
  Bomb& operator= (Bomb const&);
};

} // namespace windstille

#endif

/* EOF */
