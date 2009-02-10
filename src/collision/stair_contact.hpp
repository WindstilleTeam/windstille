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

#ifndef HEADER_STAIR_CONTACT_HPP
#define HEADER_STAIR_CONTACT_HPP

#include "contact.hpp"

class TileMap;

/** A special contact to walk stairs up or down */
class StairContact : public Contact
{
private:
  /** The tilemap with which the contact interacts */
  TileMap* tilemap;

  /** Current position in unit of tiles */
  Point pos;

  /** Amount of how much we are away from the center of the tile, its
      in range [-0.5, 0.5], else we move on to the next tile.
      advancement is in sync with the x-axis, y-axis depend on
      direction */
  float advancement;

  unsigned int tile_type;

public:
  StairContact(TileMap* tilemap, const Point& p);
  
  void update(float delta);

  /** Move the contact forward or backward across the stairs */
  void advance(float s);

  /** Get the current position in world coordinates */
  Vector get_pos() const;

  /** Return false when the given contact has left the stairs */
  bool is_active() const;

private:
  void advance_or_not();

  StairContact (const StairContact&);
  StairContact& operator= (const StairContact&);
};

#endif

/* EOF */
