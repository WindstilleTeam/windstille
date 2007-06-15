/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_CONNECTION_HPP
#define HEADER_CONNECTION_HPP

#include "math/vector.hpp"

class Segment;
class Node;

/** 
 */
class Connection
{
public:
  Segment* segment;

  /** Position on the segment, stored with range [0,1], not
      world-co */
  float pos;

  /** Move forward \a adv of units in world-co 
   *  @return The amount of units left when hitting the end of the segment or 0.0f 
   */
  float advance_this_segment(float adv);

public:
  /** Move forward \a adv of units in world-co, automatically jump
   * across segments, unless a end node is hit
   *
   * @param adv in: the amount of advancment to be done, out: the
   * amount of units that wheren't use on the given segment
   * @param next_segment out: the next segment get written into this
   *
   *  @return The amount of units left when hitting the end of the segment or 0.0f 
   */
  void advance(float& adv, Node*& next_node);

  Vector get_pos() const;
  
private:
  Connection (const Connection&);
  Connection& operator= (const Connection&);
};

#endif

/* EOF */
