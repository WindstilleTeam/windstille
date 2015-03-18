/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EDITOR_SNAP_DATA_HPP
#define HEADER_WINDSTILLE_EDITOR_SNAP_DATA_HPP

/**
 *  SnapData contains the offset that an object has to be moved by to
 *  'snap' to a neighbouring object or the grid. SnapData can be
 *  merged with other SnapData objects so that only the closest 'snap'
 *  propogates.
 */
class SnapData
{
public:
  Vector2f offset;
  bool x_set;
  bool y_set;

public:
  SnapData()
    : offset(0, 0),
      x_set(false),
      y_set(false)
  {}

  void set_x(float x)
  {
    x_set    = true;
    offset.x = x;
  }

  void set_y(float y)
  {
    y_set    = true;
    offset.y = y;
  }

  void merge(const SnapData& rhs)
  {
    if (((x_set && rhs.x_set) && (rhs.offset.x < offset.x)) ||
        (!x_set && rhs.x_set))
    {
      offset.x = rhs.offset.x;
      x_set = true;
    }

    if (((y_set && rhs.y_set) && (rhs.offset.y < offset.y)) ||
        (!y_set && rhs.y_set))
    {
      offset.y = rhs.offset.y;
      y_set = true;
    }
  }
};

#endif

/* EOF */
