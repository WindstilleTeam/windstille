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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef HEADER_SPIDER_HPP
#define HEADER_SPIDER_HPP

#include "entity.hpp"
#include "sprite2d/sprite.hpp"

class SpiderMine : public Entity
{
private:
  Sprite sprite;
  Sprite explode;
  Sprite explode_light;
  Vector initial_position;
  float walk_speed;
  float jump_time;
  enum { WAIT, ATTACK, RETURN, JUMP, EXPLODE } state;
  bool exploded;
    
protected:
  void search_for_player(float delta);

public:
  SpiderMine(FileReader& props);
  ~SpiderMine();
  
  void update(float delta);
  void draw (SceneContext& sc);
  void die();
};

#endif

/* EOF */
