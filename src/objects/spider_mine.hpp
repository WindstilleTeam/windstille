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

#ifndef HEADER_WINDSTILLE_OBJECTS_SPIDER_MINE_HPP
#define HEADER_WINDSTILLE_OBJECTS_SPIDER_MINE_HPP

#include "engine/entity.hpp"
#include "sprite2d/sprite.hpp"

class SpiderMine : public Entity
{
private:
  Sprite sprite;
  Sprite explode;
  Sprite explode_light;
  Vector2f initial_position;
  float walk_speed;
  float jump_time;
  enum { WAIT, ATTACK, RETURN, JUMP, EXPLODE } state;
  bool exploded;
    
protected:
  void search_for_player(float delta);

public:
  SpiderMine(const FileReader& props);
  ~SpiderMine();
  
  void update(float delta);
  void draw (SceneContext& sc);
  void die();
};

#endif

/* EOF */
