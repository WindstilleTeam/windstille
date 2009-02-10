/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_BOMB_HXX
#define HEADER_BOMB_HXX

#include "sprite2d/sprite.hpp"
#include "game_object.hpp"

/** */
class Bomb : public GameObject
{
private:
  Sprite sprite;
  Sprite explo;
  Sprite light;
  Sprite highlight;
  Sprite explolight;

  Vector pos;
  float count;
  enum { COUNTDOWN, EXPLODE } state;
  bool exploded;

public:
  Bomb(int x, int y);
  virtual ~Bomb();

  void update(float delta);
  void draw(SceneContext& gc);
private:
  void explode();
  Bomb (const Bomb&);
  Bomb& operator= (const Bomb&);
};

#endif

/* EOF */
