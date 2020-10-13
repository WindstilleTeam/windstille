/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_TILE_TILE_MAP_HPP
#define HEADER_WINDSTILLE_TILE_TILE_MAP_HPP

#include <string>

#include "app/globals.hpp"
#include "util/field.hpp"
#include "engine/game_object.hpp"
#include "display/scene_context.hpp"

class Tile;

class TileMap : public GameObject
{
private:
  Field<Tile*> field;
  typedef Field<Tile*>::iterator FieldIter;
  float z_pos;
  float total_time;

public:
  TileMap(ReaderMapping const& props);
  ~TileMap() override;

  void update (float delta) override;
  void draw (SceneContext& gc) override;

  /** @return the type of ground at the given world coordinates */
  bool is_ground(float x, float y);

  /** @return the type of ground at the given subtile coordinates */
  unsigned int get_pixel(int x, int y);

  int get_width () const { return field.get_width(); }
  int get_height () const { return field.get_height (); }

  int get_tile_size () const { return TILE_SIZE; }

  /** Shoots a ray from \a pos into direction \a angle, returns the
      position were the ray collides with the tilemap */
  glm::vec2 raycast(const glm::vec2& pos, float angle);
};

#endif

/* EOF */
