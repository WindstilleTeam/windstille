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

#include "tile/tile.hpp"
#include "tile/tile_map.hpp"
#include "collision/stair_contact.hpp"

StairContact::StairContact(TileMap* tilemap_, const Point& pos_)
  : tilemap(tilemap_), 
    pos(pos_), 
    advancement(0.0f),
    tile_type()
{
  unsigned int col = tilemap->get_pixel(pos.x, pos.y);
  if (!(col & TILE_STAIRS))
    std::cout << "Warning: StairContact: Given tile is no stair tile" << std::endl;

  tile_type = col;
}

void
StairContact::update(float )
{
  // shouldn't be needed!?
}

void
StairContact::advance(float s)
{
  if (s == 0)
    return;

  s /= 32.0f;

  //std::cout << "Advance: (" << pos.x << ", " << pos.y << ") " << advancement << " " << s << std::endl;
  
  float step = (s > 0.0f) ? 1.0f : -1.0f;

  // advancement must happen tile by tile  
  while ((fabs(s) > fabs(step)) && is_active())
    {
      advancement += step;
      s -= step;

      // increment tile position, based on tile direction
      // can be up/down, left/rigth
      advance_or_not();
    }

  advancement += s;
  advance_or_not();
}

void
StairContact::advance_or_not()
{
  if (tile_type & TILE_LEFT)
    {
      if (advancement < -0.5f)
        {
          pos.x -= 1;
          pos.y += 1;
          advancement += 1.0f;
        }
      else if (advancement > 0.5f)
        {
          pos.x += 1;
          pos.y -= 1;
          advancement -= 1.0f;
        }
    }
  else if (tile_type & TILE_RIGHT)
    {
      if (advancement < -0.5f)
        {
          pos.x += 1;
          pos.y -= 1;
          advancement += 1.0f;
        }
      else if (advancement > 0.5f)
        {
          pos.x -= 1;
          pos.y += 1;
          advancement -= 1.0f;
        }
    }
}

Vector2f
StairContact::get_pos() const
{
  if (tile_type & TILE_RIGHT)
  {
    return Vector2f(static_cast<float>(pos.x) * 32.0f + 16.0f + 32.0f * advancement,
                    static_cast<float>(pos.y) * 32.0f + 16.0f + 32.0f * advancement);
  }
  else // (tile_type & TILE_LEFT)
  {
    return Vector2f(static_cast<float>(pos.x) * 32.0f + 16.0f + 32.0f * advancement,
                    static_cast<float>(pos.y) * 32.0f + 16.0f - 32.0f * advancement);
  }
}

bool
StairContact::is_active() const
{
  return (tilemap->get_pixel(pos.x, pos.y) & TILE_STAIRS);
}

/* EOF */
